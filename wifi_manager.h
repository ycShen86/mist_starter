// wifi_manager.h
// Header file for WiFi connection management

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
  private:
    const char* ssid;
    const char* password;
    unsigned long connectTimeout;
    unsigned long retryDelay;

    unsigned long lastConnectAttempt;
    bool isConnecting;
    bool autoReconnect;

    // Connection state
    enum ConnectionState {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
      FAILED
    };

    ConnectionState state;

    void printConnectionInfo();

  public:
    WiFiManager(const char* ssid, const char* password,
                unsigned long timeout = 10000,
                unsigned long retry = 5000);

    // Initialize WiFi
    void begin();

    // Non-blocking WiFi connection check and management
    void update();

    // Connect to WiFi (starts connection process)
    void connect();

    // Disconnect from WiFi
    void disconnect();

    // Check if connected
    bool isConnected();

    // Get connection state
    const char* getStateString();

    // Get IP address
    String getIPAddress();

    // Get signal strength (RSSI)
    int getRSSI();

    // Enable/disable auto-reconnect
    void setAutoReconnect(bool enable);

    // Get MAC address
    String getMACAddress();
};

#endif
