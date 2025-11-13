import { initializeApp, getApps, getApp } from "firebase/app";
import { getAuth } from "firebase/auth";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: process.env.NEXT_PUBLIC_FIREBASE_API_KEY,
  authDomain: process.env.NEXT_PUBLIC_FIREBASE_AUTH_DOMAIN,
  databaseURL: process.env.NEXT_PUBLIC_FIREBASE_DATABASE_URL,
  projectId: process.env.NEXT_PUBLIC_FIREBASE_PROJECT_ID,
  storageBucket: process.env.NEXT_PUBLIC_FIREBASE_STORAGE_BUCKET,
  messagingSenderId: process.env.NEXT_PUBLIC_FIREBASE_MESSAGING_SENDER_ID,
  appId: process.env.NEXT_PUBLIC_FIREBASE_APP_ID,
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
