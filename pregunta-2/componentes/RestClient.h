/*
  RestClient.h
  ESP32 only. WiFi connection and HTTP POST of the telemetry in JSON format.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include <Arduino.h>

class RestClient {
private:
  const char* ssid;
  const char* password;
  const char* endpoint;

public:
  RestClient(const char* ssid, const char* password, const char* endpoint);
  bool connect();
  bool isConnected() const;
  bool postJson(const String& json);
};

#endif
