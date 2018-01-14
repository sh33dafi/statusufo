#include <ESP8266WiFi.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>

#define MQTT_SERVER      "10.0.1.161"
#define MQTT_SERVERPORT  1883
#define MQTT_USERNAME    ""
#define MQTT_PASSWORD    ""

#define LED_PIN 5

Ticker ticker;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);
Adafruit_MQTT_Subscribe testFeed = Adafruit_MQTT_Subscribe(&mqtt, "/feeds/test");

void blinkStatusLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000);  // wait 10 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void callback(uint32_t status) {
  Serial.println(status);
  digitalWrite(LED_PIN, status);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  ticker.attach_ms(500, blinkStatusLed);
  delay(10);
  Serial.begin(115200);
  delay(10);

  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.autoConnect();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  ticker.detach();
  ticker.attach_ms(100, blinkStatusLed);

  testFeed.setCallback(callback);
  mqtt.subscribe(&testFeed);

  delay(1000);
  ticker.detach();
  delay(10);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);
  if (! mqtt.ping()) {
    mqtt.disconnect();
  }
}
