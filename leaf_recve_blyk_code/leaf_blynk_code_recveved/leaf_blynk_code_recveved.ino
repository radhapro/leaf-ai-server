//  // Second ESP32 — UART Receiver
// // Connect: ESP32-CAM GPIO14 (TX) → This ESP32 GPIO16 (RX)
// // Also connect GND of both boards together!

// #define UART_RX_PIN  16
// #define UART_TX_PIN  17
// #define UART_BAUD    9600

// HardwareSerial SerialUART(2);

// int leafPercent = 0;

// void setup() {
//   Serial.begin(115200);
//   SerialUART.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
//   Serial.println("=== ESP32 UART Receiver Ready ===");
// }

// void loop() {
//   if (SerialUART.available()) {
//     String received = SerialUART.readStringUntil('\n');
//     received.trim();

//     if (received.length() > 0) {
//       leafPercent = received.toInt();

//       Serial.print("Leaf Health Received: ");
//       Serial.print(leafPercent);
//       Serial.println("%");

//       if (leafPercent >= 70) {
//         Serial.println("Status: EXCELLENT HEALTH");
//       }
//       else if (leafPercent >= 40) {
//         Serial.println("Status: EARLY INFECTION / YELLOWING");
//       }
//       else if (leafPercent > 10) {
//         Serial.println("Status: SEVERE DISEASE / DRYING");
//       }
//       else {
//         Serial.println("Status: NO LEAF / DEAD");
//       }

//       Serial.println("-----------------------------");
//     }
//   }
// }