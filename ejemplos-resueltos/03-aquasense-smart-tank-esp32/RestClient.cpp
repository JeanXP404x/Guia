/*
  RestClient.cpp
  Connects to WiFi with a timeout and sends JSON with HTTPClient.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#include "RestClient.h"
#include <WiFi.h>
#include <HTTPClient.h>

namespace {
  const unsigned long CONNECTION_TIMEOUT_MS = 10000;
  const unsigned long RETRY_DELAY_MS = 250;
  const int HTTP_SUCCESS_MIN = 200;
  const int HTTP_SUCCESS_MAX = 299;
}

RestClient::RestClient(const char* ssid, const char* password, const char* endpoint)
  : ssid(ssid), password(password), endpoint(endpoint) {}

bool RestClient::connect() {
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < CONNECTION_TIMEOUT_MS) {
    delay(RETRY_DELAY_MS);
  }
  Serial.println(isConnected() ? "[WiFi] Connected." : "[WiFi] Not connected, working offline.");
  return isConnected();
}

bool RestClient::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

bool RestClient::postJson(const String& json) {
  if (!isConnected()) return false;
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  int statusCode = http.POST(json);
  http.end();
  Serial.printf("[REST] POST %s -> HTTP %d\n", json.c_str(), statusCode);
  return statusCode >= HTTP_SUCCESS_MIN && statusCode <= HTTP_SUCCESS_MAX;
}
