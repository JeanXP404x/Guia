/*
  WiFiConnection.cpp
  Connects with a timeout so the shower keeps working offline.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#include "WiFiConnection.h"
#include <WiFi.h>

namespace {
  const unsigned long CONNECTION_TIMEOUT_MS = 10000;
  const unsigned long RETRY_DELAY_MS = 250;
}

bool WiFiConnection::connect() {
  Serial.printf("[WiFi] Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (!isConnected() && millis() - start < CONNECTION_TIMEOUT_MS) delay(RETRY_DELAY_MS);
  Serial.println(isConnected() ? "[WiFi] Connected." : "[WiFi] Not connected, working offline.");
  return isConnected();
}

bool WiFiConnection::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}
