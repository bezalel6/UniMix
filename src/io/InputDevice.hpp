#pragma once

#include <Arduino.h>
#include <functional>
#include <memory>

// Base class for all input devices
class InputDevice {
   public:
    enum class DeviceType {
        ENCODER,
        BUTTON,
        JOYSTICK,
        POTENTIOMETER,
        CUSTOM
    };

    InputDevice(const String& deviceId, DeviceType type)
        : id(deviceId), type(type), initialized(false) {}

    virtual ~InputDevice() = default;

    // Pure virtual methods that must be implemented by derived classes
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;
    virtual bool hasNewInput() = 0;
    virtual void clearInputFlags() = 0;

    // Common interface methods
    const String& getId() const { return id; }
    DeviceType getType() const { return type; }
    bool isInitialized() const { return initialized; }

   protected:
    String id;
    DeviceType type;
    bool initialized;
};

// Template for type-safe device access
template <typename T>
class TypedInputDevice : public InputDevice {
   public:
    TypedInputDevice(const String& deviceId, DeviceType type)
        : InputDevice(deviceId, type) {}

    virtual ~TypedInputDevice() = default;

    // Type-safe casting method
    T* as() { return static_cast<T*>(this); }
    const T* as() const { return static_cast<const T*>(this); }
};