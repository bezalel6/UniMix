#pragma once

#include "InputDevice.hpp"
#include <ESP32Encoder.h>
#include <functional>
#include <memory>

class RotaryEncoder : public TypedInputDevice<RotaryEncoder> {
   public:
    // Callback types
    using EncoderCallback = std::function<void(int delta)>;
    using ButtonCallback = std::function<void(bool pressed)>;

    struct Config {
        int pinA;
        int pinB;
        int buttonPin;
        bool reversed;
        bool enablePullups;
        unsigned long debounceTime;
        bool hasButton;

        Config() : pinA(32), pinB(33), buttonPin(25), reversed(false), enablePullups(true), debounceTime(50), hasButton(true) {}
    };

    RotaryEncoder(const String& deviceId, const Config& config);
    ~RotaryEncoder() override;

    // InputDevice interface
    bool initialize() override;
    void shutdown() override;
    void update() override;
    bool hasNewInput() override;
    void clearInputFlags() override;

    // Encoder-specific methods
    long getPosition() const;
    int getDelta() const;
    void resetPosition();
    void setPosition(long position);

    // Button methods (if enabled)
    bool isButtonPressed() const;
    bool wasButtonPressed();
    bool wasButtonReleased();

    // Configuration methods
    void setReversed(bool reversed);
    void setButtonDebounceTime(unsigned long debounceMs);

    // Callback methods
    void setEncoderCallback(EncoderCallback callback);
    void setButtonCallback(ButtonCallback callback);

    // Static factory method for easy creation
    static std::unique_ptr<RotaryEncoder> create(const String& deviceId, const Config& config);

   private:
    Config config;
    ESP32Encoder encoder;

    // Encoder state
    long lastPosition;
    int delta;
    bool newEncoderInput;

    // Button state (if enabled)
    bool buttonState;
    bool lastButtonState;
    bool buttonPressed;
    bool buttonReleased;
    unsigned long lastButtonChange;
    bool newButtonInput;

    // Callbacks
    EncoderCallback encoderCallback;
    ButtonCallback buttonCallback;

    // Private methods
    void setupEncoder();
    void setupButton();
    void updateEncoder();
    void updateButton();
    bool readButtonRaw();
};