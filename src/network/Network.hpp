#pragma once

#include <WiFi.h>
#include <WiFiClient.h>

// Network status enumeration
enum class NetworkStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    FAILED,
    RECONNECTING
};

class Network {
   private:
    // Private constructor to prevent direct instantiation
    Network();

    // Static instance pointer
    static Network* instance;

    // Delete copy constructor and assignment operator
    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;

   public:
    // Public destructor
    ~Network();

    // Static method to get the singleton instance
    static Network& getInstance();

    // Static method to check if instance exists
    static bool hasInstance();

    // Static method to destroy the instance
    static void destroyInstance();

    // Network lifecycle methods
    void initialize();
    void shutdown();
    void update();

    // WiFi connection methods
    bool connect();
    bool connect(const char* ssid, const char* password);
    void disconnect();
    bool reconnect();

    // Status methods
    NetworkStatus getStatus() const;
    const char* getStatusString() const;
    bool isConnected() const;
    String getLocalIP() const;
    String getSSID() const;
    int getRSSI() const;
    String getMACAddress() const;

    // Connection monitoring
    unsigned long getConnectedTime() const;
    unsigned long getLastReconnectAttempt() const;
    int getReconnectAttempts() const;

    // Configuration
    void setAutoReconnect(bool enable);
    void setReconnectInterval(unsigned long intervalMs);
    void setTimeout(unsigned long timeoutMs);

    // Event callbacks (optional)
    typedef void (*NetworkEventCallback)(NetworkStatus status);
    void setEventCallback(NetworkEventCallback callback);

   private:
    // Internal state
    bool initialized;
    NetworkStatus status;
    bool autoReconnect;
    unsigned long connectionTimeout;
    unsigned long reconnectInterval;
    unsigned long lastConnectionAttempt;
    unsigned long lastReconnectAttempt;
    unsigned long connectionStartTime;
    int reconnectAttempts;

    // WiFi credentials (from secret.h)
    String wifiSSID;
    String wifiPassword;

    // Event callback
    NetworkEventCallback eventCallback;

    // Internal methods
    void setupWiFi();
    void handleWiFiEvent();
    void updateConnectionStatus();
    void attemptReconnection();
    void setNewStatus(NetworkStatus newStatus);

    // Configuration constants
    static const unsigned long DEFAULT_CONNECTION_TIMEOUT = 10000;  // 10 seconds
    static const unsigned long DEFAULT_RECONNECT_INTERVAL = 5000;   // 5 seconds
    static const int MAX_RECONNECT_ATTEMPTS = 5;
};
