#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncTCP.h>
#include <Hash.h>

AsyncWebServer webServer(80);
AsyncMqttClient mqttClient;

void onMqttConnect(bool sessionPresent) {
  Serial.println("** Connected to the broker **");
  uint16_t packetIdSub = mqttClient.subscribe("ficklampa/#", 0);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.print("received topic: ");
  Serial.println(topic);

  if (strcmp(topic, "ficklampa/on") == 0) {
    digitalWrite(LED_BUILTIN, 0);
  } else if (strcmp(topic, "ficklampa/off") == 0) {
    digitalWrite(LED_BUILTIN, 1);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  Serial.begin(115200);
  // try connecting to wifi
  Serial.print("\nwifi connecting ");
  WiFi.begin("gnejs", "badborste");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    // log success and connect to mqtt setServer
    Serial.print("wifi connected "); Serial.println(WiFi.localIP());

    // client.setServer(prefs.mqtt_server, 1883);
    // client.setCallback(callback);
    webServer.on("/v", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      response->print("Battery ");
      response->println(getBatteryVoltage());
      request->send(response);
    });

    webServer.begin();

    mqttClient.onConnect(onMqttConnect);
    // mqttClient.onDisconnect(onMqttDisconnect);
    // mqttClient.onSubscribe(onMqttSubscribe);
    // mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    // mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(IPAddress(192, 168, 0, 36), 1883);
    mqttClient.setKeepAlive(5).setCleanSession(false).setClientId("ficklampa");
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
  }
}

float getBatteryVoltage() {
  return analogRead(A0) * (3.2 / 1023.0) * (3.72/1.74);
}

void loop() {
}
