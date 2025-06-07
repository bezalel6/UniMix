#include "IO.hpp"
#include <Arduino.h>
#include <type_traits>
#include <memory>

// Initialize static member
IO* IO::instance = nullptr;

// Private constructor
IO::IO() : initialized(false) {
    // Constructor implementation
}

// Destructor
IO::~IO() {
    if (initialized) {
        shutdown();
    }
}

// Get singleton instance
IO& IO::getInstance() {
    if (instance == nullptr) {
        instance = new IO();
    }
    return *instance;
}

// Check if instance exists
bool IO::hasInstance() {
    return instance != nullptr;
}

// Destroy the singleton instance
void IO::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

// Initialize the IO system
void IO::initialize() {
    if (!initialized) {
        // Initialize all devices
        for (auto& device : devices) {
            device->initialize();
        }
        initialized = true;
    }
}

// Shutdown the IO system
void IO::shutdown() {
    if (initialized) {
        // Shutdown all devices
        for (auto& device : devices) {
            device->shutdown();
        }
        initialized = false;
    }
}

// Update IO state - call this regularly in main loop or timer
void IO::update() {
    if (!initialized) return;

    // Update all devices and check for new input
    for (auto& device : devices) {
        device->update();

        // Call global callback if device has new input
        if (device->hasNewInput() && globalCallback) {
            globalCallback(device->getId(), device->getType());
        }
    }
}

// Device management methods
template <typename T>
T* IO::addDevice(std::unique_ptr<T> device) {
    if (!device) return nullptr;

    // Check if device ID already exists
    if (hasDevice(device->getId())) {
        return nullptr;  // Device ID must be unique
    }

    T* rawPtr = device.get();
    size_t index = devices.size();

    devices.push_back(std::move(device));
    deviceMap[rawPtr->getId()] = index;

    // If already initialized, initialize the new device
    if (initialized) {
        rawPtr->initialize();
    }

    return rawPtr;
}

InputDevice* IO::getDevice(const String& deviceId) {
    auto it = deviceMap.find(deviceId);
    if (it != deviceMap.end()) {
        return devices[it->second].get();
    }
    return nullptr;
}

bool IO::removeDevice(const String& deviceId) {
    auto it = deviceMap.find(deviceId);
    if (it != deviceMap.end()) {
        size_t indexToRemove = it->second;

        // Shutdown device before removal
        devices[indexToRemove]->shutdown();

        // Remove from vector (this invalidates indices)
        devices.erase(devices.begin() + indexToRemove);

        // Rebuild deviceMap since indices changed
        deviceMap.clear();
        for (size_t i = 0; i < devices.size(); ++i) {
            deviceMap[devices[i]->getId()] = i;
        }

        return true;
    }
    return false;
}

bool IO::hasDevice(const String& deviceId) {
    return deviceMap.find(deviceId) != deviceMap.end();
}

// Convenience methods for common device types
RotaryEncoder* IO::addRotaryEncoder(const String& deviceId, const RotaryEncoder::Config& config) {
    return addDevice(std::unique_ptr<RotaryEncoder>(new RotaryEncoder(deviceId, config)));
}

Button* IO::addButton(const String& deviceId, const Button::Config& config) {
    return addDevice(std::unique_ptr<Button>(new Button(deviceId, config)));
}

RotaryEncoder* IO::getRotaryEncoder(const String& deviceId) {
    auto it = deviceMap.find(deviceId);
    if (it != deviceMap.end()) {
        InputDevice* device = devices[it->second].get();
        if (device->getType() == InputDevice::DeviceType::ENCODER) {
            return static_cast<RotaryEncoder*>(device);
        }
    }
    return nullptr;
}

Button* IO::getButton(const String& deviceId) {
    auto it = deviceMap.find(deviceId);
    if (it != deviceMap.end()) {
        InputDevice* device = devices[it->second].get();
        if (device->getType() == InputDevice::DeviceType::BUTTON) {
            return static_cast<Button*>(device);
        }
    }
    return nullptr;
}

// Get devices by type
std::vector<RotaryEncoder*> IO::getRotaryEncoders() {
    std::vector<RotaryEncoder*> result;
    for (auto& device : devices) {
        if (device->getType() == InputDevice::DeviceType::ENCODER) {
            result.push_back(static_cast<RotaryEncoder*>(device.get()));
        }
    }
    return result;
}

std::vector<Button*> IO::getButtons() {
    std::vector<Button*> result;
    for (auto& device : devices) {
        if (device->getType() == InputDevice::DeviceType::BUTTON) {
            result.push_back(static_cast<Button*>(device.get()));
        }
    }
    return result;
}

// Template method for getting devices of specific type
template <typename T>
std::vector<T*> IO::getDevicesOfType() {
    std::vector<T*> result;
    for (auto& device : devices) {
        T* typedDevice = dynamic_cast<T*>(device.get());
        if (typedDevice) {
            result.push_back(typedDevice);
        }
    }
    return result;
}

// Global input state methods
bool IO::hasNewInput() {
    for (auto& device : devices) {
        if (device->hasNewInput()) {
            return true;
        }
    }
    return false;
}

void IO::clearAllInputFlags() {
    for (auto& device : devices) {
        device->clearInputFlags();
    }
}

// Device iteration
std::vector<InputDevice*> IO::getAllDevices() {
    std::vector<InputDevice*> result;
    for (auto& device : devices) {
        result.push_back(device.get());
    }
    return result;
}

std::vector<String> IO::getDeviceIds() {
    std::vector<String> result;
    for (auto& device : devices) {
        result.push_back(device->getId());
    }
    return result;
}

// Event system
void IO::setGlobalInputCallback(GlobalInputCallback callback) {
    globalCallback = callback;
}

// Private helper methods
size_t IO::findDeviceIndex(const String& deviceId) {
    auto it = deviceMap.find(deviceId);
    return (it != deviceMap.end()) ? it->second : SIZE_MAX;
}

// Explicit template instantiations for common types
template RotaryEncoder* IO::addDevice<RotaryEncoder>(std::unique_ptr<RotaryEncoder> device);
template Button* IO::addDevice<Button>(std::unique_ptr<Button> device);
template std::vector<RotaryEncoder*> IO::getDevicesOfType<RotaryEncoder>();
template std::vector<Button*> IO::getDevicesOfType<Button>();
