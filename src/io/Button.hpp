#pragma once

#include "InputDevice.hpp"
#include <functional>
#include <memory>

class Button : public TypedInputDevice<Button> {
   public:
    // Callback type
    using ButtonCallback = std::function<void(bool pressed)>;

    struct Config {
        int pin;
        bool enablePullup;
        bool activeLow;  // True for pullup buttons, false for pulldown
        unsigned long debounceTime;

        Config() : pin(0), enablePullup(true), activeLow(true), debounceTime(50) {}
    };

    Button(const String& deviceId, const Config& config);
    ~Button() override;

    // InputDevice interface
    bool initialize() override;
    void shutdown() override;
    void update() override;
    bool hasNewInput() override;
    void clearInputFlags() override;

    // Button-specific methods
    bool isPressed() const;
    bool wasPressed();
    bool wasReleased();
    unsigned long getPressedDuration() const;
    unsigned long getReleasedDuration() const;

    // Configuration methods
    void setDebounceTime(unsigned long debounceMs);

    // Callback methods
    void setCallback(ButtonCallback callback);

    // Static factory method
    static std::unique_ptr<Button> create(const String& deviceId, const Config& config);

   private:
    Config config;

    // Button state
    bool currentState;
    bool lastState;
    bool stateChanged;
    bool pressed;
    bool released;
    unsigned long lastStateChange;
    unsigned long lastPressed;
    unsigned long lastReleased;
    bool newInput;

    // Callback
    ButtonCallback callback;

    // Private methods
    void setupButton();
    void updateButton();
    bool readButtonRaw();
};