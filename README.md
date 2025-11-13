# Proyecto IoT: Contador de Productos con ESP32 y Next.js

Este proyecto es una solución IoT de extremo a extremo que utiliza un microcontrolador (ESP32) para detectar productos con un sensor de movimiento PIR y una aplicación web (Next.js) como dashboard de monitoreo y control en tiempo real, utilizando Firebase como backend centralizado.

## Arquitectura de la Solución

*   **Dispositivo IoT (Rol: Cliente/Emisor):** Un ESP32 con un sensor de movimiento PIR. Es responsable de detectar un producto que pasa y reportarlo a Firebase.
*   **Backend (Rol: Broker de Datos):** Un proyecto de Firebase que utiliza Realtime Database para la comunicación en tiempo real y Firebase Authentication para la seguridad.
*   **Interfaz de Usuario (Rol: Dashboard/Receptor):** Una aplicación web desarrollada con Next.js y ShadCN/UI que permite a los usuarios monitorear el contador y enviar comandos.

---

## ✅ Checklist de Implementación y Evaluación

A continuación se detalla el progreso del proyecto basado en los requisitos de evaluación.

### Fase 1: Configuración del Backend (Firebase)

| Estado | Tarea | Notas |
| :---: | :--- | :--- |
| ✅ | **1.1. Crear Proyecto en Firebase** | El proyecto está creado e inicializado. |
| ✅ | **1.2. Habilitar Servicios** | **Authentication** y **Realtime Database** están activados y configurados en la app. |
| ✅ | **1.3. Definir Estructura de Datos en RTDB** | La estructura está definida y en uso: `products/count` y `commands/reset`. |
| ✅ | **1.4. Implementar Reglas de Seguridad** | Las reglas están listas y **han sido aplicadas** en la consola de Firebase. |

**Reglas de Seguridad Aplicadas (Realtime Database):**
```json
{
  "rules": {
    "products": {
      "count": {
        ".read": "auth != null",
        ".write": "auth != null",
        ".validate": "newData.isNumber()"
      }
    },
    "commands": {
      "reset": {
        ".read": "auth != null",
        ".write": "auth != null && newData.hasChildren(['.sv'])"
      }
    },
    ".read": false,
    ".write": false
  }
}
```

---
### Fase 2: Desarrollo del Dispositivo IoT (ESP32)

| Estado | Tarea | Notas |
| :---: | :--- | :--- |
| 🟡 | **2.1. Conexión a la Red** | El código está listo, pero **PENDIENTE** de cargar en el ESP32 con las credenciales WiFi correctas. |
| 🟡 | **2.2. Integración con Firebase** | El código está listo, solo falta cargarlo en el microcontrolador. |
| 🟡 | **2.3. Lógica del Sensor PIR** | El código está implementado, falta conectar el sensor físicamente y probar. |
| 🟡 | **2.4. Recepción de Comandos** | La suscripción al comando de reinicio está programada, falta probarla en el hardware. |
| 🟡 | **2.5. Almacenamiento Temporal (Offline)**| La lógica de almacenamiento offline está implementada en el código, falta probarla en un escenario real. |

---
### Fase 3: Desarrollo de la Aplicación Web (Next.js)

| Estado | Tarea | Notas |
| :---: | :--- | :--- |
| ✅ | **3.1. UI y Conexión a Firebase** | La interfaz de usuario está creada (`counter-card.tsx`) y conectada a Firebase. |
| ✅ | **3.2. Autenticación de Usuario (Seguridad)** | El flujo de login con Email/Contraseña y Google está 100% funcional. Las vistas están protegidas. |
| ✅ | **3.3. Monitoreo en Tiempo Real** | La app se suscribe a `products/count` y actualiza la UI en tiempo real. |
| ✅ | **3.4. Control Remoto** | El botón "Reset Counter" escribe la señal de reinicio en `commands/reset` correctamente. |
| ✅ | **3.5. Alertas y Lógica de Negocio** | Se muestra una alerta visual y una notificación (`toast`) cuando el contador alcanza el límite. |
| ❌ | **3.6. Almacenamiento Temporal (Offline)**| **PENDIENTE:** No hay lógica de almacenamiento local para acciones offline en la app web. |
| ✅ | **3.7. Compatibilidad** | La aplicación es una interfaz web funcional y compatible con navegadores modernos. |

---
### Fase 4: Guía de Puesta en Marcha del Hardware

Esta sección explica cómo configurar el ESP32.

#### **Paso 1: Instalar el Entorno de Desarrollo (Arduino IDE)**

1.  **Descarga Arduino IDE:** Ve al [sitio web oficial de Arduino](https://www.arduino.cc/en/software) y descarga la versión 2.x.
2.  **Añade Soporte para ESP32:**
    *   Abre el IDE, ve a `Archivo` > `Preferencias`.
    *   En el campo "Gestor de URLs de Tarjetas Adicionales", pega: `https://dl.espressif.com/dl/package_esp32_index.json`
    *   Ve a `Herramientas` > `Placa` > `Gestor de Tarjetas`, busca "esp32" e instala el paquete de **Espressif Systems**.

#### **Paso 2: Instalar Librerías Necesarias**

1.  Abre `Herramientas` > `Gestionar Librerías...`.
2.  Busca e instala **`Firebase ESP32 Client`** por Mobizt. Acepta instalar todas las dependencias que solicite.

#### **Paso 3: Conectar el Hardware (Pines)**

Conecta el sensor PIR al ESP32 de la siguiente manera:

*   **Pin VCC del Sensor PIR** -> **Pin 3V3 del ESP32**
*   **Pin GND del Sensor PIR** -> **Pin GND del ESP32**
*   **Pin OUT del Sensor PIR** -> **Pin GPIO 27 del ESP32**

*(El código usa el pin 27, si usas otro, actualiza la variable `PIR_PIN` en `firmware/esp32-firmware.cpp`)*.

#### **Paso 4: Configurar y Cargar el Código**

1.  **Abre el código:** Abre el archivo `firmware/esp32-firmware.cpp` con el Arduino IDE.
2.  **Modifica las credenciales WiFi:** Reemplaza `"TU_WIFI_SSID"` y `"TU_WIFI_PASSWORD"` con los datos de tu red. Las credenciales de Firebase ya están configuradas.
3.  **Conecta el ESP32:** Usa un cable USB para conectar el ESP32 a tu computadora.
4.  **Selecciona la Placa y el Puerto:**
    *   En `Herramientas` > `Placa`, elige "ESP32 Dev Module".
    *   En `Herramientas` > `Puerto`, selecciona el puerto que apareció (ej. `COM3` o `/dev/ttyUSB0`).
5.  **Carga el código:** Presiona el botón "Subir" (→).
6.  **Monitorea:** Abre el `Monitor Serie` (`Herramientas` > `Monitor Serie`) y configúralo a **115200 baudios** para ver los mensajes.
