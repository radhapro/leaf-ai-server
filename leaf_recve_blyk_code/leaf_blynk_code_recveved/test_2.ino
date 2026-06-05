  #define BLYNK_TEMPLATE_ID   "TMPL38dKi_w43"
#define BLYNK_TEMPLATE_NAME "Smart Plant Health Monitor"
#define BLYNK_AUTH_TOKEN    "EbzzLnlcHRqo5N3wJsWeE6BZreky5yOC"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char ssid[] = "abcd1234";
char pass[] = "123456789";

// UART
#define UART_RX_PIN   16
#define UART_TX_PIN   17
#define UART_BAUD     9600

// DHT11
#define DHT_PIN       4
#define DHT_TYPE      DHT11

// Soil Moisture
#define SOIL_PIN      34

// LEDs
#define LED_GREEN     18
#define LED_YELLOW    19
#define LED_RED       23

// OLED
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

HardwareSerial SerialUART(2);
DHT dht(DHT_PIN, DHT_TYPE);
BlynkTimer timer;

int leafPercent = 0;
String leafStatus = "";
float temperature = 0;
float humidity = 0;
int soilPercent = 0;

// -----------------------------------------------
// Update OLED Display
// -----------------------------------------------
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Line 1 — Temp + Humidity
  display.setCursor(0, 0);
  display.print("T:");
  display.print((int)temperature);
  display.print("C  H:");
  display.print((int)humidity);
  display.print("%");

  // Line 2 — Soil Moisture
  display.setCursor(0, 11);
  display.print("Soil: ");
  display.print(soilPercent);
  display.print("%");

  // Line 3 — Leaf Status
  display.setCursor(0, 22);
  display.print("Leaf: ");
  display.print(leafStatus);

  display.display();
}

// -----------------------------------------------
// Set LED based on leaf condition
// -----------------------------------------------
void updateLEDs() {
  if (leafPercent >= 70) {
    digitalWrite(LED_GREEN,  HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED,    LOW);
  }
  else if (leafPercent > 10) {
    digitalWrite(LED_GREEN,  LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED,    LOW);
  }
  else {
    digitalWrite(LED_GREEN,  LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED,    HIGH);
  }
}

// -----------------------------------------------
// Read DHT11 + Soil and send all to Blynk
// -----------------------------------------------
void sendToBlynk() {
  // --- DHT11 ---
  temperature = dht.readTemperature();
  humidity    = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("[DHT11] FAIL - Could not read sensor!");
  } else {
    Serial.println("------------------------------------------");
    Serial.print("[DHT11] Temperature : ");
    Serial.print(temperature);
    Serial.println(" C");
    Serial.print("[DHT11] Humidity    : ");
    Serial.print(humidity);
    Serial.println(" %");

    if (Blynk.connected()) {
      Blynk.virtualWrite(V0, temperature);
      Blynk.virtualWrite(V1, humidity);
      Serial.println("[BLYNK] OK - Sent T=" + String(temperature) + " | H=" + String(humidity));
    }
  }

  // --- Soil Moisture ---
  int soilRaw = analogRead(SOIL_PIN);
  soilPercent = map(soilRaw, 4095, 0, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  Serial.print("[SOIL ] Moisture    : ");
  Serial.print(soilPercent);
  Serial.println(" %");

  if (Blynk.connected()) {
    Blynk.virtualWrite(V2, soilPercent);
    Serial.println("[BLYNK] OK - Sent Soil V2=" + String(soilPercent) + "%");
  }

  // --- Leaf ---
  if (Blynk.connected()) {
    Blynk.virtualWrite(V4, leafPercent);
    Serial.println("[BLYNK] OK - Sent Leaf V4=" + String(leafPercent) + "%");
  } else {
    Serial.println("[BLYNK] FAIL - Not connected! Skipping send...");
  }

  // --- Update OLED ---
  updateOLED();

  Serial.println("------------------------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("==========================================");
  Serial.println("  Smart Plant Health Monitor Starting     ");
  Serial.println("==========================================");

  // LED pins
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    LOW);
  Serial.println("[LED  ] Green=18  Yellow=19  Red=23 - Ready");

  // OLED Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[OLED ] FAIL - Display not found!");
  } else {
    Serial.println("[OLED ] OK - Display Started!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 10);
    display.println("Plant Monitor");
    display.setCursor(30, 22);
    display.println("Starting...");
    display.display();
    delay(1500);
  }

  // DHT11
  dht.begin();
  Serial.println("[DHT11] Started on pin " + String(DHT_PIN));

  // Soil
  Serial.println("[SOIL ] Moisture sensor on pin " + String(SOIL_PIN));

  // UART
  SerialUART.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  Serial.println("[UART ] Initialized RX=" + String(UART_RX_PIN) + " TX=" + String(UART_TX_PIN));

  // WiFi
  Serial.println("------------------------------------------");
  Serial.print("[WiFi ] Connecting to: ");
  Serial.println(ssid);

  // Show on OLED while connecting
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.println(ssid);
  display.display();

  WiFi.begin(ssid, pass);
  int wifiRetry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiRetry++;
    if (wifiRetry >= 20) {
      Serial.println();
      Serial.println("[WiFi ] FAIL - Could not connect! Restarting...");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WiFi FAILED!");
      display.println("Restarting...");
      display.display();
      delay(2000);
      ESP.restart();
    }
  }

  Serial.println();
  Serial.println("[WiFi ] OK - Connected Successfully!");
  Serial.print("[WiFi ] IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.print("[WiFi ] RSSI       : ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // Show IP on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.println(WiFi.localIP().toString());
  display.display();
  delay(1500);

  // Blynk
  Serial.println("------------------------------------------");
  Serial.println("[BLYNK] Connecting to Blynk server...");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting Blynk...");
  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (Blynk.connected()) {
    Serial.println("[BLYNK] OK - Connected to Blynk Successfully!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Blynk Connected!");
    display.println("System Ready!");
    display.display();
    delay(1500);
  } else {
    Serial.println("[BLYNK] FAIL - Check your Auth Token!");
  }

  // Timer - every 2 seconds
  timer.setInterval(2000L, sendToBlynk);

  Serial.println("------------------------------------------");
  Serial.println("[UART ] Ready to receive data from ESP32-CAM!");
  Serial.println("[BLYNK] Sending all sensor values every 2 sec");
  Serial.println("[OLED ] Display running!");
  Serial.println("==========================================");
}

void loop() {
  Blynk.run();
  timer.run();

  // Read UART from ESP32-CAM
  if (SerialUART.available()) {
    String received = SerialUART.readStringUntil('\n');
    received.trim();

    if (received.length() > 0) {
      leafPercent = received.toInt();

      if (leafPercent >= 70) {
        leafStatus = "EXCELLENT";
      }
      else if (leafPercent >= 40) {
        leafStatus = "INFECTION";
      }
      else if (leafPercent > 10) {
        leafStatus = "DRYING";
      }
      else {
        leafStatus = "DEAD";
        leafPercent = 0;
      }

      updateLEDs();
      updateOLED();

      Serial.println("------------------------------------------");
      Serial.print("[UART ] OK - Leaf Received : ");
      Serial.print(leafPercent);
      Serial.println("%");
      Serial.println("[LEAF ] Status            : " + leafStatus);
      Serial.println("[LED  ] Updated");
      Serial.println("[OLED ] Updated");
      Serial.println("------------------------------------------");
    }
  }
}
 