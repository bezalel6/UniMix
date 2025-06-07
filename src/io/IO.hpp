#pragma once

#include "InputDevice.hpp"
#include "RotaryEncoder.hpp"
#include "Button.hpp"
#include <vector>
#include <memory>
#include <map>
#include <functional>

class IO {
   private:
    // Private constructor to prevent direct instantiation
    IO();

    // Static instance pointer
    static IO* instance;

    // Delete copy constructor and assignment operator
    IO(const IO&) = delete;
    IO& operator=(const IO&) = delete;

   public:
    // Public destructor
    ~IO();

    // Static method to get the singleton instance
    static IO& getInstance();

    // Static method to check if instance exists
    static bool hasInstance();

    // Static method to destroy the instance
    static void destroyInstance();

    // IO lifecycle methods
    void initialize();
    void shutdown();
    void update();

    // Device management methods
    template <typename T>
    T* addDevice(std::unique_ptr<T> device);

    InputDevice* getDevice(const String& deviceId);
    bool removeDevice(const String& deviceId);
    bool hasDevice(const String& deviceId);

    // Convenience methods for common device types
    RotaryEncoder* addRotaryEncoder(const String& deviceId, const RotaryEncoder::Config& config = {});
    Button* addButton(const String& deviceId, const Button::Config& config);

    RotaryEncoder* getRotaryEncoder(const String& deviceId);
    Button* getButton(const String& deviceId);

    // Get devices by type
    std::vector<RotaryEncoder*> getRotaryEncoders();
    std::vector<Button*> getButtons();

    // Template method for getting devices of specific type
    template <typename T>
    std::vector<T*> getDevicesOfType();

    // Global input state methods
    bool hasNewInput();
    void clearAllInputFlags();

    // Device iteration
    std::vector<InputDevice*> getAllDevices();
    std::vector<String> getDeviceIds();

    // Event system
    using GlobalInputCallback = std::function<void(const String& deviceId, InputDevice::DeviceType type)>;
    void setGlobalInputCallback(GlobalInputCallback callback);

   private:
    // Device storage
    std::vector<std::unique_ptr<InputDevice>> devices;
    std::map<String, size_t> deviceMap;  // deviceId -> index in devices vector

    bool initialized;
    GlobalInputCallback globalCallback;

    // Helper methods
    size_t findDeviceIndex(const String& deviceId);
};
