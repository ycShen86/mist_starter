// wifi_manager.cpp
// Implementation of WiFi connection management

#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password,
                         unsigned long timeout, unsigned long retry) {
  this->ssid = ssid;
  this->password = password;
  this->connectTimeout = timeout;
  this->retryDelay = retry;
  this->lastConnectAttempt = 0;
  this->isConnecting = false;
  this->autoReconnect = true;
  this->state = DISCONNECTED;
}

void WiFiManager::begin() {
  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);

  Serial.println("WiFi Manager Initialized");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Disable auto-reconnect to handle it manually
  WiFi.setAutoReconnect(false);

  // Start connection attempt
  connect();
}

void WiFiManager::connect() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  state = CONNECTING;
  isConnecting = true;
  lastConnectAttempt = millis();
}

void WiFiManager::disconnect() {
  WiFi.disconnect();
  state = DISCONNECTED;
  isConnecting = false;
  Serial.println("WiFi disconnected");
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

const char* WiFiManager::getStateString() {
  switch (state) {
    case DISCONNECTED: return "Disconnected";
    case CONNECTING: return "Connecting";
    case CONNECTED: return "Connected";
    case FAILED: return "Failed";
    default: return "Unknown";
  }
}

String WiFiManager::getIPAddress() {
  if (isConnected()) {
    return WiFi.localIP().toString();
  }
  return "Not connected";
}

int WiFiManager::getRSSI() {
  if (isConnected()) {
    return WiFi.RSSI();
  }
  return 0;
}

void WiFiManager::setAutoReconnect(bool enable) {
  autoReconnect = enable;
  Serial.print("Auto-reconnect: ");
  Serial.println(enable ? "Enabled" : "Disabled");
}

String WiFiManager::getMACAddress() {
  return WiFi.macAddress();
}

void WiFiManager::printConnectionInfo() {
  Serial.println("==========================================");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println("==========================================");
}

void WiFiManager::update() {
  unsigned long currentTime = millis();
  wl_status_t wifiStatus = WiFi.status();

  switch (state) {
    case CONNECTING:
      if (wifiStatus == WL_CONNECTED) {
        // Successfully connected
        state = CONNECTED;
        isConnecting = false;
        printConnectionInfo();
      } else if (currentTime - lastConnectAttempt >= connectTimeout) {
        // Connection timeout
        Serial.println("WiFi connection timeout");
        state = FAILED;
        isConnecting = false;

        // Retry if auto-reconnect is enabled
        if (autoReconnect) {
          Serial.print("Retrying in ");
          Serial.print(retryDelay / 1000);
          Serial.println(" seconds...");
          lastConnectAttempt = currentTime;
        }
      }
      break;

    case CONNECTED:
      if (wifiStatus != WL_CONNECTED) {
        // Lost connection
        Serial.println("WiFi connection lost!");
        state = DISCONNECTED;

        // Attempt reconnection if auto-reconnect is enabled
        if (autoReconnect) {
          lastConnectAttempt = currentTime;
        }
      }
      break;

    case FAILED:
    case DISCONNECTED:
      if (autoReconnect && (currentTime - lastConnectAttempt >= retryDelay)) {
        // Attempt to reconnect
        connect();
      }
      break;
  }
}
