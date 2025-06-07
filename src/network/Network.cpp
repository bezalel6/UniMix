#include "network.hpp"
#include "../../include/secret.h"

// Initialize static instance pointer
Network* Network::instance = nullptr;

// Private constructor
Network::Network() : initialized(false),
                     status(NetworkStatus::DISCONNECTED),
                     autoReconnect(true),
                     connectionTimeout(DEFAULT_CONNECTION_TIMEOUT),
                     reconnectInterval(DEFAULT_RECONNECT_INTERVAL),
                     lastConnectionAttempt(0),
                     lastReconnectAttempt(0),
                     connectionStartTime(0),
                     reconnectAttempts(0),
                     wifiSSID(WIFI_SSID),
                     wifiPassword(WIFI_PASS),
                     eventCallback(nullptr) {
}

// Destructor
Network::~Network() {
    shutdown();
}

// Static method to get the singleton instance
Network& Network::getInstance() {
    if (instance == nullptr) {
        instance = new Network();
    }
    return *instance;
}

// Static method to check if instance exists
bool Network::hasInstance() {
    return instance != nullptr;
}

// Static method to destroy the instance
void Network::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

// Initialize the network system
void Network::initialize() {
    if (initialized) return;

    Serial.println("Network: Initializing...");

    setupWiFi();
    initialized = true;

    Serial.println("Network: Initialized");
}

// Shutdown the network system
void Network::shutdown() {
    if (!initialized) return;

    Serial.println("Network: Shutting down...");

    disconnect();
    initialized = false;

    Serial.println("Network: Shutdown complete");
}

// Update network status and handle reconnection
void Network::update() {
    if (!initialized) return;

    updateConnectionStatus();

    // Handle auto-reconnection
    if (autoReconnect && status == NetworkStatus::DISCONNECTED) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt >= reconnectInterval) {
            if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
                Serial.println("Network: Attempting auto-reconnection...");
                setNewStatus(NetworkStatus::RECONNECTING);
                attemptReconnection();
            }
        }
    }
}

// Connect to WiFi using credentials from secret.h
bool Network::connect() {
    return connect(wifiSSID.c_str(), wifiPassword.c_str());
}

// Connect to WiFi with custom credentials
bool Network::connect(const char* ssid, const char* password) {
    if (!initialized) return false;

    Serial.print("Network: Connecting to ");
    Serial.print(ssid);
    Serial.println("...");

    setNewStatus(NetworkStatus::CONNECTING);
    lastConnectionAttempt = millis();
    connectionStartTime = lastConnectionAttempt;

    WiFi.begin(ssid, password);

    // Wait for connection or timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED &&
           (millis() - startTime) < connectionTimeout) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        setNewStatus(NetworkStatus::CONNECTED);
        reconnectAttempts = 0;

        Serial.println("Network: Connected!");
        Serial.print("Network: IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Network: RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");

        return true;
    } else {
        setNewStatus(NetworkStatus::FAILED);
        Serial.println("Network: Connection failed!");
        return false;
    }
}

// Disconnect from WiFi
void Network::disconnect() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Network: Disconnecting...");
        WiFi.disconnect();
    }
    setNewStatus(NetworkStatus::DISCONNECTED);
}

// Reconnect to WiFi
bool Network::reconnect() {
    disconnect();
    delay(100);
    return connect();
}

// Get current network status
NetworkStatus Network::getStatus() const {
    return status;
}

// Get network status as string
const char* Network::getStatusString() const {
    switch (status) {
        case NetworkStatus::DISCONNECTED:
            return "Disconnected";
        case NetworkStatus::CONNECTING:
            return "Connecting";
        case NetworkStatus::CONNECTED:
            return "Connected";
        case NetworkStatus::FAILED:
            return "Failed";
        case NetworkStatus::RECONNECTING:
            return "Reconnecting";
        default:
            return "Unknown";
    }
}

// Check if connected
bool Network::isConnected() const {
    return status == NetworkStatus::CONNECTED && WiFi.status() == WL_CONNECTED;
}

// Get local IP address
String Network::getLocalIP() const {
    if (isConnected()) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

// Get connected SSID
String Network::getSSID() const {
    if (isConnected()) {
        return WiFi.SSID();
    }
    return wifiSSID;
}

// Get signal strength
int Network::getRSSI() const {
    if (isConnected()) {
        return WiFi.RSSI();
    }
    return 0;
}

// Get MAC address
String Network::getMACAddress() const {
    return WiFi.macAddress();
}

// Get connected time in milliseconds
unsigned long Network::getConnectedTime() const {
    if (isConnected() && connectionStartTime > 0) {
        return millis() - connectionStartTime;
    }
    return 0;
}

// Get last reconnect attempt time
unsigned long Network::getLastReconnectAttempt() const {
    return lastReconnectAttempt;
}

// Get number of reconnect attempts
int Network::getReconnectAttempts() const {
    return reconnectAttempts;
}

// Set auto-reconnect behavior
void Network::setAutoReconnect(bool enable) {
    autoReconnect = enable;
    Serial.print("Network: Auto-reconnect ");
    Serial.println(enable ? "enabled" : "disabled");
}

// Set reconnect interval
void Network::setReconnectInterval(unsigned long intervalMs) {
    reconnectInterval = intervalMs;
    Serial.print("Network: Reconnect interval set to ");
    Serial.print(intervalMs);
    Serial.println(" ms");
}

// Set connection timeout
void Network::setTimeout(unsigned long timeoutMs) {
    connectionTimeout = timeoutMs;
    Serial.print("Network: Connection timeout set to ");
    Serial.print(timeoutMs);
    Serial.println(" ms");
}

// Set event callback
void Network::setEventCallback(NetworkEventCallback callback) {
    eventCallback = callback;
}

// Private methods

// Setup WiFi configuration
void Network::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // Handle reconnection manually

    Serial.println("Network: WiFi setup complete");
}

// Handle WiFi events
void Network::handleWiFiEvent() {
    // This method can be expanded to handle WiFi events
    // For now, status is updated in updateConnectionStatus()
}

// Update connection status based on WiFi state
void Network::updateConnectionStatus() {
    wl_status_t wifiStatus = WiFi.status();

    switch (wifiStatus) {
        case WL_CONNECTED:
            if (status != NetworkStatus::CONNECTED) {
                setNewStatus(NetworkStatus::CONNECTED);
                reconnectAttempts = 0;
            }
            break;

        case WL_DISCONNECTED:
            if (status == NetworkStatus::CONNECTED ||
                status == NetworkStatus::CONNECTING) {
                setNewStatus(NetworkStatus::DISCONNECTED);
            }
            break;

        case WL_CONNECT_FAILED:
            setNewStatus(NetworkStatus::FAILED);
            break;

        case WL_CONNECTION_LOST:
            setNewStatus(NetworkStatus::DISCONNECTED);
            break;

        default:
            // Handle other states as needed
            break;
    }
}

// Attempt reconnection
void Network::attemptReconnection() {
    lastReconnectAttempt = millis();
    reconnectAttempts++;

    Serial.print("Network: Reconnection attempt ");
    Serial.print(reconnectAttempts);
    Serial.print("/");
    Serial.println(MAX_RECONNECT_ATTEMPTS);

    if (reconnect()) {
        Serial.println("Network: Reconnection successful!");
    } else {
        Serial.println("Network: Reconnection failed");
        if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
            Serial.println("Network: Max reconnection attempts reached");
        }
    }
}

// Set new status and trigger callback
void Network::setNewStatus(NetworkStatus newStatus) {
    if (status != newStatus) {
        NetworkStatus oldStatus = status;
        status = newStatus;

        Serial.print("Network: Status changed from ");
        Serial.print(getStatusString());
        Serial.print(" to ");
        Serial.println(getStatusString());

        // Trigger callback if set
        if (eventCallback) {
            eventCallback(status);
        }
    }
}
