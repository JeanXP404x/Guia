/*
  WiFiConnection.h
  WiFi link of the shower (Moen Smart Water Network app integration).

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

class WiFiConnection {
private:
  const char* ssid;
  const char* password;

public:
  WiFiConnection(const char* ssid, const char* password) : ssid(ssid), password(password) {}
  bool connect();
  bool isConnected() const;
};

#endif
