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
// ¡CORRECCIÓN! Este es el encabezado correcto para la v4.x de la librería
#include <Firebase_ESP_Client.h>

// --- CALLBACKS PARA EVENTOS DE FIREBASE ---
void streamCallback(StreamData data);
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
unsigned long debounceDelay = 1000; // 1 segundo de anti-rebote

// 4. OBJETOS DE FIREBASE
FirebaseData fbdo;       // Objeto para operaciones generales
FirebaseData stream;     // Objeto para el listener del comando de reseteo
FirebaseAuth auth;       // Objeto para la autenticación
FirebaseConfig config;   // Objeto para la configuración

// 5. VARIABLES DE ESTADO Y OFFLINE
bool firebaseReady = false;
int offlineCount = 0;
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 30000; // 30 segundos

// --- FUNCIÓN DE INTERRUPCIÓN ---
void IRAM_ATTR detectsMovement() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    motionDetected = true;
    lastDebounceTime = millis();
  }
}

// --- CONFIGURACIÓN INICIAL ---
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando ESP32 - Contador de Productos IoT...");

  pinMode(PIR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), detectsMovement, RISING);

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

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Asignar callbacks al objeto de stream
  stream.setStreamCallback(streamCallback, streamTimeoutCallback);

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Iniciar la escucha del comando de reseteo
  if (!Firebase.RTDB().beginStream(&stream, "/commands/reset")) {
    Serial.println("------------------------------------");
    Serial.println("Error iniciando Stream: " + stream.errorReason());
    Serial.println("------------------------------------");
  }

  Serial.println("Setup finalizado. Esperando eventos...");
}

// --- BUCLE PRINCIPAL ---
void loop() {
  if (Firebase.ready() && !firebaseReady) {
    firebaseReady = true;
    Serial.println("Firebase listo para operar.");

    if (offlineCount > 0) {
      Serial.printf("Sincronizando %d conteos offline...\n", offlineCount);
      if (Firebase.RTDB().addInt(&fbdo, "/products/count", offlineCount)) {
        offlineCount = 0;
      } else {
        Serial.println("Error al sincronizar conteos offline: " + fbdo.errorReason());
      }
    }
  }

  if (!Firebase.ready()) {
      firebaseReady = false;
  }

  if (millis() - lastCheckTime > checkInterval) {
    lastCheckTime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado. Intentando reconectar...");
    } else if (!Firebase.ready()) {
      Serial.println("Firebase no está listo. Verifique la conexión y las credenciales.");
    }
  }

  if (motionDetected) {
    motionDetected = false;
    Serial.println("¡Movimiento detectado!");

    if (firebaseReady) {
      Serial.println("Incrementando contador en Firebase...");
      if (Firebase.RTDB().addInt(&fbdo, "/products/count", 1)) {
        Serial.println("Contador incrementado con éxito.");
      } else {
        Serial.println("Error al incrementar: " + fbdo.errorReason());
        offlineCount++;
      }
    } else {
      offlineCount++;
      Serial.printf("Conexión perdida. Conteos offline acumulados: %d\n", offlineCount);
    }
  }
}

// --- DEFINICIÓN DE CALLBACKS ---

void streamCallback(StreamData data) {
  Serial.println("Evento de Stream recibido!");
  Serial.printf("Ruta: %s, Evento: %s, Dato: %s\n", data.streamPath().c_str(), data.eventType().c_str(), data.payload().c_str());

  if (data.streamPath() == "/reset" && data.eventType() == "put") {
    if(data.payload() == "true") { // Opcional: chequear el valor
        Serial.println("Comando de reseteo recibido.");
        if (Firebase.RTDB().setInt(&fbdo, "/products/count", 0)) {
            Serial.println("Contador reseteado a 0 en Firebase.");
            offlineCount = 0;
        } else {
            Serial.println("Error al resetear el contador: " + fbdo.errorReason());
        }
    }
  }
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, el listener se detuvo. Intentando reiniciar...");
  }
}
