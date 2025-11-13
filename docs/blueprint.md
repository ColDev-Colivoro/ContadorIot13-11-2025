# **App Name**: Contador IoT

## Core Features:

- Increment Counter: ESP32 increments the counter value in Realtime Database upon detecting a product (e.g., button press).
- Real-time Display: Flutter app displays the counter value in real time using Streams.
- Remote Reset: Flutter app sends a 'Reset' command to Realtime Database, which the ESP32 listens to and resets the counter.
- Alert Notifications: The app shows an alert if the counter reaches a predefined limit.
- Authentication: Firebase Authentication is implemented to restrict access to authenticated users only.
- Temporary Storage: Implement temporary local storage on both ESP32 and Flutter, for synchronization when connectivity is restored.

## Style Guidelines:

- Primary color: A deep, saturated blue (#3F51B5), evokes reliability, clarity, and trust.
- Background color: A very light blue-gray (#F0F4F8) maintains a clean, unobtrusive backdrop.
- Accent color: A muted purple (#7E57C2), will highlight key interactive elements and alerts.
- Body: 'PT Sans' for body text due to its clear legibility and modern humanist style
- Headers: 'Space Grotesk', gives a modern look which will pair well with 'PT Sans'.
- Use clean, minimalist icons to represent actions like 'reset' and notifications.
- Simple, intuitive layout with the counter prominently displayed. Clear button placement for commands.
- Subtle transitions for real-time updates to the counter, and engaging alert animations when limits are reached.