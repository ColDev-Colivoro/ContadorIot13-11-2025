import { initializeApp, getApps, getApp } from "firebase/app";
import { getAuth } from "firebase/auth";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  "projectId": "studio-4285007076-74e1c",
  "appId": "1:1006165106392:web:d11308b0cca7e8e57481ad",
  "apiKey": "AIzaSyAkmjWdqUElqp4l8XB56COpF6fGMZx4mD0",
  "authDomain": "studio-4285007076-74e1c.firebaseapp.com",
  "measurementId": "",
  "messagingSenderId": "1006165106392",
  "storageBucket": "studio-4285007076-74e1c.appspot.com",
  "databaseURL": "https://studio-4285007076-74e1c-default-rtdb.firebaseio.com"
};

// Initialize Firebase
const app = !getApps().length ? initializeApp(firebaseConfig) : getApp();
const auth = getAuth(app);
const db = getDatabase(app);

export { app, auth, db };

/*
Recommended Firebase Realtime Database Security Rules:

These rules enforce that only authenticated users can read or write data.
The `count` value must be a number, and the `reset` command must be a boolean.

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
        // Here we allow writing a timestamp using server-side value
        ".write": "auth != null && newData.hasChildren(['.sv'])",
      }
    },
    // Deny all other read/write operations
    ".read": false,
    ".write": false
  }
}

*/
