#include <WiFi.h>
#include <PubSubClient.h>

// ===== Wi-Fi credentials =====
#define WIFI_SSID "cslab"
#define WIFI_PASS "aksesg31"

// ===== MQTT broker =====
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883

#define MQTT_RAIN_TOPIC        "streetlight/rain"
#define MQTT_LDR_AMBIENT_TOPIC "streetlight/ldr_ambient"
#define MQTT_LDR_LED_TOPIC     "streetlight/ldr_led"
#define MQTT_LED_FAULT_TOPIC   "streetlight/led_fault"

// ===== Pins =====
const int rainPin   = 4;     // Rain sensor (LOW = rain)
const int ledPin1   = 9;
const int ledPin2   = 6;
const int ledPin3   = 8;
const int lightPin  = 10;    // Ambient LDR
const int ldrLedPin = 14;    // LDR under LED (fault detection)

// ===== Thresholds =====
const int darkThreshold  = 3000;
const int dimBrightness  = 178;   // 70%
const int maxBrightness  = 255;   // 100%
const int ledFaultThresh = 2000;
const int sampleCount    = 5;

WiFiClient espClient;
PubSubClient client(espClient);

// ===== Wi-Fi =====
void setupWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===== MQTT =====
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32-StreetLight")) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, retrying...");
      delay(2000);
    }
  }
}

// ===== Average ADC read =====
int readAverage(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / samples;
}

// ===== LED fault detection =====
bool detectLedFault(int ledBrightness, int ledLdrValue) {
  if (ledBrightness == 0) return false;
  return (ledLdrValue > ledFaultThresh);
}

void setup() {
  Serial.begin(115200);

  pinMode(rainPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  setupWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // ===== Read sensors =====
  int rainDetected  = digitalRead(rainPin);      // LOW = rain
  int ambientLdr    = analogRead(lightPin);      // Ambient light
  int ledLdrValue   = readAverage(ldrLedPin, sampleCount); // LED LDR

  bool isDark    = ambientLdr >= darkThreshold;
  bool isRaining = rainDetected == LOW;

  // ===== LED brightness logic =====
  int ledBrightness = 0;

  if (isDark && isRaining) {
    ledBrightness = maxBrightness;   // Night + rain
  }
  else if (isDark || isRaining) {
    ledBrightness = dimBrightness;   // Night OR rain
  }
  else {
    ledBrightness = 0;               // Day + no rain
  }

  // ===== Apply PWM =====
  analogWrite(ledPin1, ledBrightness);
  analogWrite(ledPin2, ledBrightness);
  analogWrite(ledPin3, ledBrightness);

  // ===== Fault detection =====
  bool ledFault = detectLedFault(ledBrightness, ledLdrValue);

  // ===== MQTT publish =====
  client.publish(MQTT_RAIN_TOPIC, isRaining ? "1" : "0");
  client.publish(MQTT_LDR_AMBIENT_TOPIC, String(ambientLdr).c_str());
  client.publish(MQTT_LDR_LED_TOPIC, String(ledLdrValue).c_str());
  client.publish(MQTT_LED_FAULT_TOPIC, ledFault ? "1" : "0");

  // ===== Serial debug =====
  Serial.print("Ambient LDR: "); Serial.print(ambientLdr);
  Serial.print(" | LED LDR: "); Serial.print(ledLdrValue);
  Serial.print(" | Rain: "); Serial.print(isRaining ? "YES" : "NO");
  Serial.print(" | PWM: "); Serial.print(ledBrightness);
  Serial.print(" | FAULT: "); Serial.println(ledFault ? "YES" : "NO");

  delay(1000);
}
