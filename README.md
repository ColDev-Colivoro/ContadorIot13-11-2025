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
| 🟡 | **1.4. Implementar Reglas de Seguridad** | Las reglas están definidas en `src/config/firebase.ts`, pero **deben ser copiadas y aplicadas** en la consola de Firebase para ser efectivas. |

**Acción Requerida:**
1.  Ve a tu consola de Firebase -> Realtime Database -> Pestaña "Rules".
2.  Pega las reglas que se encuentran comentadas en `src/config/firebase.ts`.

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
| ❌ | **2.1. Conexión a la Red** | **PENDIENTE:** Programar la conexión WiFi y la reconexión automática en el ESP32. |
| ❌ | **2.2. Integración con Firebase** | **PENDIENTE:** Usar `Firebase-ESP-Client` para conectar el ESP32 al backend. |
| ❌ | **2.3. Lógica del Sensor PIR** | **PENDIENTE:** Leer el sensor PIR e incrementar de forma atómica el valor en la ruta `products/count`. |
| ❌ | **2.4. Recepción de Comandos** | **PENDIENTE:** Suscribirse a `commands/reset` para recibir la señal de reinicio desde la app. |
| ❌ | **2.5. Almacenamiento Temporal (Offline)**| **PENDIENTE:** Implementar lógica para guardar conteos localmente si se pierde la conexión y sincronizarlos después. |

---
### Fase 3: Desarrollo de la Aplicación Web (Next.js)

| Estado | Tarea | Notas |
| :---: | :--- | :--- |
| ✅ | **3.1. UI y Conexión a Firebase** | La interfaz de usuario está creada (`counter-card.tsx`) y conectada a Firebase. |
| ✅ | **3.2. Autenticación de Usuario (Seguridad)** | El flujo de login con Email/Contraseña y Google está 100% funcional. Las vistas están protegidas. |
| ✅ | **3.3. Monitoreo en Tiempo Real** | La app se suscribe a `products/count` y actualiza la UI en tiempo real. |
| ✅ | **3.4. Control Remoto** | El botón "Reset Counter" escribe la señal de reinicio en `commands/reset` correctamente. |
| ✅ | **3.5. Alertas y Lógica de Negocio** | Se muestra una alerta visual y una notificación (`toast`) cuando el contador alcanza el límite. |
| ❌ | **3.6. Almacenamiento Temporal (Offline)**| **PENDIENTE:** No hay lógica de almacenamiento local para acciones offline. |
| ✅ | **3.7. Compatibilidad** | La aplicación es una interfaz web funcional y compatible con navegadores modernos. |

---

### Resumen de Tareas Pendientes

1.  **Firebase:**
    *   [ ] Aplicar las Reglas de Seguridad en la consola de Firebase.
2.  **ESP32 (Toda la implementación):**
    *   [ ] Escribir el código completo para el ESP32: conexión WiFi, conexión a Firebase, lectura del sensor PIR, escritura del contador y suscripción al comando de reinicio.
    *   [ ] Implementar la lógica de almacenamiento offline en el microcontrolador.
3.  **App Web (Next.js):**
    *   [ ] (Opcional/Avanzado) Implementar almacenamiento temporal para comandos si la app se desconecta.
