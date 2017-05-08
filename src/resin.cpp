#include "resin.h"

Resin::Resin(){}
Resin::~Resin(){}

void Resin::Setup(String applicationUUID, String ssid, String password) {
    _applicationUUID = applicationUUID;
    _ssid = ssid;
    _password = password;

    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(_ssid.c_str(), _password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }

    _httpServer = ESP8266WebServer(80);
    _httpUpdater = ESP8266HTTPUpdateServer();

    _httpServer.on("/id", HTTP_GET, [&]() {
        _httpServer.send(200, "text/plain", _applicationUUID);
    });

    _httpServer.on("/env", HTTP_POST, [&]() {
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(_httpServer.arg(0));
        // It is up to the user to process the environment variables
        // https://bblanchon.github.io/ArduinoJson/
        _httpServer.send(200, "text/plain", "");
    });

    _httpServer.begin();
    _httpUpdater.setup(&_httpServer);
}

void Resin::Loop() {
    _httpServer.handleClient();

    if (WiFi.isConnected()) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}
