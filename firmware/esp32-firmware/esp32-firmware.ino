/**
 * @file esp32-firmware.ino
 * @author Tu Nombre
 * @brief Firmware para el ESP32 que funciona como un contador de productos IoT.
 *
 * Funcionalidades:
 * 1.  Se conecta a WiFi y a Firebase Realtime Database.
 * 2.  Utiliza un sensor de movimiento PIR para detectar productos.
 * 3.  Incrementa un contador en Firebase de forma atómica (`/products/count`).
 * 4.  Escucha un comando de reinicio desde la aplicación web (`/commands/reset`).
 * 5.  Implementa reconexión automática a WiFi y Firebase.
 * 6.  Cuenta con una lógica simple de almacenamiento offline si se pierde la conexión.
 *
 * Pines de Conexión (Ejemplo):
 * - Sensor PIR (OUT) -> GPIO 27
 */

// 1. INCLUIR LIBRERÍAS
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// --- CALLBACKS PARA EVENTOS DE FIREBASE ---
void streamCallback(FirebaseStream data);
void streamTimeoutCallback(bool timeout);

// 2. CONFIGURACIÓN DE ACCESO (REEMPLAZAR CON TUS DATOS)
#define WIFI_SSID "TU_WIFI_SSID" // <--- REEMPLAZA ESTO
#define WIFI_PASSWORD "TU_WIFI_PASSWORD" // <--- REEMPLAZA ESTO
#define API_KEY "AIzaSyAkmjWdqUElqp4l8XB56COpF6fGMZx4mD0" // ¡LISTO!
#define DATABASE_URL "https://studio-4285007076-74e1c-default-rtdb.firebaseio.com" // ¡LISTO!

// 3. PINES Y VARIABLES GLOBALES
#define PIR_PIN 27       // Pin donde está conectado el sensor PIR
volatile bool motionDetected = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000; // 1 segundo de anti-rebote para no contar el mismo objeto múltiples veces

// 4. OBJETOS DE FIREBASE
FirebaseData fbdo;         // Objeto para operaciones Firebase (get, set, etc.)
FirebaseData stream;       // Objeto para el listener (Stream) del comando de reseteo
FirebaseAuth auth;         // Objeto para la autenticación
FirebaseConfig config;     // Objeto para la configuración de Firebase

// 5. VARIABLES DE ESTADO Y OFFLINE
bool firebaseReady = false;
int offlineCount = 0; // Contador para cuando no hay conexión
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 30000; // Revisa conexión cada 30 segundos

// --- FUNCIÓN DE INTERRUPCIÓN ---
// Se activa cuando el pin del sensor PIR cambia de estado (LOW a HIGH)
void IRAM_ATTR detectsMovement() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    motionDetected = true;
    lastDebounceTime = millis();
  }
}

// --- CONFIGURACIÓN INICIAL (se ejecuta una vez) ---
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando ESP32 - Contador de Productos IoT...");

  // Configurar el pin del sensor PIR como entrada
  pinMode(PIR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), detectsMovement, RISING);

  // Conexión a WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Configuración de Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Asignar los callbacks para el Stream
  config.stream_callback = streamCallback;
  config.stream_timeout_callback = streamTimeoutCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Iniciar la escucha del comando de reseteo
  if (!Firebase.beginStream(stream, "/commands/reset")) {
    Serial.println("------------------------------------");
    Serial.println("Error iniciando Stream: " + stream.errorReason());
    Serial.println("------------------------------------");
  }

  Serial.println("Setup finalizado. Esperando eventos...");
}


// --- BUCLE PRINCIPAL (se ejecuta repetidamente) ---
void loop() {
  // Primero, revisa si Firebase está listo
  if (Firebase.ready() && !firebaseReady) {
    firebaseReady = true;
    Serial.println("Firebase listo para operar.");
    
    // Si había conteos offline, sincronízalos ahora
    if (offlineCount > 0) {
        Serial.printf("Sincronizando %d conteos offline...\n", offlineCount);
        Firebase.RTDB.increment(&fbdo, "/products/count", offlineCount);
        offlineCount = 0; // Resetear contador offline
    }
  } else if (!Firebase.ready()) {
    firebaseReady = false;
  }

  // Comprobar estado de conexión periódicamente para informar
  if (millis() - lastCheckTime > checkInterval) {
    lastCheckTime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado. Intentando reconectar...");
    } else if (!Firebase.ready()) {
      Serial.println("Firebase no está listo. Verifique la conexión y las credenciales.");
    }
  }

  // Si se detectó movimiento (manejado por la interrupción)
  if (motionDetected) {
    motionDetected = false; // Resetear la bandera
    Serial.println("¡Movimiento detectado!");

    if (firebaseReady) {
      // Si hay conexión, incrementa el contador en Firebase
      Serial.println("Incrementando contador en Firebase...");
      // Usamos `increment` para una operación atómica
      if (Firebase.RTDB.increment(&fbdo, "/products/count", 1)) {
        Serial.println("Contador incrementado con éxito: " + fbdo.stringData());
      } else {
        Serial.println("Error al incrementar: " + fbdo.errorReason());
        offlineCount++; // Si falla, lo guardamos para después
      }
    } else {
      // Si no hay conexión, incrementa el contador local
      offlineCount++;
      Serial.printf("Conexión perdida. Conteos offline acumulados: %d\n", offlineCount);
    }
  }
}

// --- DEFINICIÓN DE CALLBACKS ---

/**
 * @brief Callback que se ejecuta cuando llega un dato del Stream de Firebase.
 * Aquí se maneja el comando de reseteo.
 */
void streamCallback(FirebaseStream data) {
  Serial.println("Evento de Stream recibido!");
  Serial.printf("Ruta: %s\nTipo de evento: %s\n", data.streamPath().c_str(), data.eventType().c_str());

  // Si el evento ocurre en la ruta que nos interesa
  if (data.streamPath() == "/reset" && data.eventType() == "put") {
    Serial.println("Señal de reseteo recibida desde la app.");
    
    // Ponemos el contador en Firebase a 0
    if (Firebase.RTDB.setInt(&fbdo, "/products/count", 0)) {
        Serial.println("Contador reseteado a 0 en Firebase.");
        offlineCount = 0; // También resetea el contador offline
    } else {
        Serial.println("Error al resetear el contador: " + fbdo.errorReason());
    }
  }
}

/**
 * @brief Callback que se ejecuta si el Stream se desconecta o agota el tiempo.
 */
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, el listener se detuvo.");
  }
}
