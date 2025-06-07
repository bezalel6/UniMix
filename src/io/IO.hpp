#pragma once

#include <ESP32Encoder.h>

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

    // Rotary encoder methods
    long getEncoderPosition();
    int getEncoderDelta();
    void resetEncoder();
    bool isEncoderButtonPressed();
    bool wasEncoderButtonPressed();   // Returns true once per press
    bool wasEncoderButtonReleased();  // Returns true once per release

    // Input state methods
    bool hasNewInput();
    void clearInputFlags();

    // Encoder configuration
    void setEncoderPins(int pinA, int pinB, int buttonPin);
    void setEncoderReversed(bool reversed);
    void enablePullups(bool enable);

    // Debouncing configuration
    void setButtonDebounceTime(unsigned long debounceMs);

    // Events callback (optional)
    typedef void (*EncoderCallback)(int delta);
    typedef void (*ButtonCallback)(bool pressed);
    void setEncoderCallback(EncoderCallback callback);
    void setButtonCallback(ButtonCallback callback);

   private:
    // Rotary encoder instance
    ESP32Encoder encoder;

    // Pin definitions
    int encoderPinA;
    int encoderPinB;
    int buttonPin;

    // Internal state
    bool initialized;
    long lastEncoderPosition;
    int encoderDelta;
    bool encoderReversed;
    bool pullupsEnabled;

    // Button state management
    bool buttonState;
    bool lastButtonState;
    bool buttonPressed;
    bool buttonReleased;
    unsigned long lastButtonChange;
    unsigned long buttonDebounceTime;

    // Input flags
    bool newInputAvailable;

    // Callbacks
    EncoderCallback encoderCallback;
    ButtonCallback buttonCallback;

    // Internal methods
    void setupEncoder();
    void setupButton();
    void updateEncoder();
    void updateButton();
    bool readButtonRaw();

    // Default pin assignments (can be changed via setEncoderPins)
    static const int DEFAULT_ENCODER_PIN_A = 32;
    static const int DEFAULT_ENCODER_PIN_B = 33;
    static const int DEFAULT_BUTTON_PIN = 25;
    static const unsigned long DEFAULT_DEBOUNCE_TIME = 50;  // milliseconds
};
