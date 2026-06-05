//  #define BLYNK_TEMPLATE_ID   "TMPL38dKi_w43"
// #define BLYNK_TEMPLATE_NAME "Smart Plant Health Monitor"
// #define BLYNK_AUTH_TOKEN    "EbzzLnlcHRqo5N3wJsWeE6BZreky5yOC"

// #include <WiFi.h>
// #include <BlynkSimpleEsp32.h>

// char ssid[] = "abcd1234";      // <- change this
// char pass[] = "123456789";  // <- change this

// #define UART_RX_PIN  16
// #define UART_TX_PIN  17
// #define UART_BAUD    9600

// HardwareSerial SerialUART(2);

// int leafPercent = 0;
// String leafStatus = "";

// BlynkTimer timer;

// void sendToBlynk() {
//   if (Blynk.connected()) {
//     Blynk.virtualWrite(V4, leafPercent);
//     Serial.println("[BLYNK] OK - Data sent to Blynk V3 = " + String(leafPercent) + "%");
//   } else {
//     Serial.println("[BLYNK] FAIL - Not connected to Blynk! Skipping send...");
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   Serial.println("==========================================");
//   Serial.println("  ESP32 UART Receiver + Blynk Starting   ");
//   Serial.println("==========================================");

//   // UART Init
//   SerialUART.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
//   Serial.println("[UART] Initialized RX=" + String(UART_RX_PIN) + " TX=" + String(UART_TX_PIN) + " Baud=" + String(UART_BAUD));

//   // WiFi Connecting
//   Serial.println("------------------------------------------");
//   Serial.print("[WiFi] Connecting to: ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, pass);
//   int wifiRetry = 0;
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//     wifiRetry++;
//     if (wifiRetry >= 20) {
//       Serial.println();
//       Serial.println("[WiFi] FAIL - Could not connect! Check SSID/Password.");
//       Serial.println("[WiFi] Restarting ESP32...");
//       delay(2000);
//       ESP.restart();
//     }
//   }

//   Serial.println();
//   Serial.println("[WiFi] OK - Connected Successfully!");
//   Serial.print("[WiFi] IP Address: ");
//   Serial.println(WiFi.localIP());
//   Serial.print("[WiFi] Signal Strength (RSSI): ");
//   Serial.print(WiFi.RSSI());
//   Serial.println(" dBm");

//   // Blynk Connecting
//   Serial.println("------------------------------------------");
//   Serial.println("[BLYNK] Connecting to Blynk server...");
//   Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

//   if (Blynk.connected()) {
//     Serial.println("[BLYNK] OK - Connected to Blynk Successfully!");
//   } else {
//     Serial.println("[BLYNK] FAIL - Check your Auth Token!");
//   }

//   // Blynk Timer - send every 2 seconds
//   timer.setInterval(2000L, sendToBlynk);

//   // Ready Message
//   Serial.println("------------------------------------------");
//   Serial.println("[UART ] Ready to receive data from ESP32-CAM!");
//   Serial.println("[BLYNK] Sending leaf value to Blynk every 2 sec");
//   Serial.println("==========================================");
// }

// void loop() {
//   Blynk.run();
//   timer.run();

//   if (SerialUART.available()) {
//     String received = SerialUART.readStringUntil('\n');
//     received.trim();

//     if (received.length() > 0) {
//       leafPercent = received.toInt();

//       if (leafPercent >= 70) {
//         leafStatus = "EXCELLENT HEALTH";
//       }
//       else if (leafPercent >= 40) {
//         leafStatus = "EARLY INFECTION / YELLOWING";
//       }
//       else if (leafPercent > 10) {
//         leafStatus = "SEVERE DISEASE / DRYING";
//       }
//       else {
//         leafStatus = "NO LEAF / DEAD";
//         leafPercent = 0;
//       }

//       Serial.println("------------------------------------------");
//       Serial.print("[UART ] OK - Data Received from ESP32-CAM: ");
//       Serial.print(leafPercent);
//       Serial.println("%");
//       Serial.println("[LEAF ] Status: " + leafStatus);
//       Serial.println("------------------------------------------");
//     }
//   }
// }