#include "IO.hpp"
#include <Arduino.h>

// Initialize static member
IO* IO::instance = nullptr;

// Private constructor
IO::IO() : encoderPinA(DEFAULT_ENCODER_PIN_A),
           encoderPinB(DEFAULT_ENCODER_PIN_B),
           buttonPin(DEFAULT_BUTTON_PIN),
           initialized(false),
           lastEncoderPosition(0),
           encoderDelta(0),
           encoderReversed(false),
           pullupsEnabled(true),
           buttonState(false),
           lastButtonState(false),
           buttonPressed(false),
           buttonReleased(false),
           lastButtonChange(0),
           buttonDebounceTime(DEFAULT_DEBOUNCE_TIME),
           newInputAvailable(false),
           encoderCallback(nullptr),
           buttonCallback(nullptr) {
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
        setupEncoder();
        setupButton();
        initialized = true;
    }
}

// Shutdown the IO system
void IO::shutdown() {
    if (initialized) {
        initialized = false;
    }
}

// Update IO state - call this regularly in main loop or timer
void IO::update() {
    if (!initialized) return;

    updateEncoder();
    updateButton();
}

// Rotary encoder methods
long IO::getEncoderPosition() {
    if (!initialized) return 0;
    return encoder.getCount();
}

int IO::getEncoderDelta() {
    if (!initialized) return 0;
    return encoderDelta;
}

void IO::resetEncoder() {
    if (!initialized) return;
    encoder.clearCount();
    lastEncoderPosition = 0;
    encoderDelta = 0;
}

bool IO::isEncoderButtonPressed() {
    return buttonState;
}

bool IO::wasEncoderButtonPressed() {
    bool result = buttonPressed;
    buttonPressed = false;  // Clear flag after reading
    return result;
}

bool IO::wasEncoderButtonReleased() {
    bool result = buttonReleased;
    buttonReleased = false;  // Clear flag after reading
    return result;
}

// Input state methods
bool IO::hasNewInput() {
    return newInputAvailable;
}

void IO::clearInputFlags() {
    newInputAvailable = false;
    buttonPressed = false;
    buttonReleased = false;
    encoderDelta = 0;
}

// Encoder configuration
void IO::setEncoderPins(int pinA, int pinB, int buttonPin) {
    if (initialized) return;  // Can't change pins after initialization

    encoderPinA = pinA;
    encoderPinB = pinB;
    this->buttonPin = buttonPin;
}

void IO::setEncoderReversed(bool reversed) {
    encoderReversed = reversed;
}

void IO::enablePullups(bool enable) {
    pullupsEnabled = enable;
}

// Debouncing configuration
void IO::setButtonDebounceTime(unsigned long debounceMs) {
    buttonDebounceTime = debounceMs;
}

// Events callback
void IO::setEncoderCallback(EncoderCallback callback) {
    encoderCallback = callback;
}

void IO::setButtonCallback(ButtonCallback callback) {
    buttonCallback = callback;
}

// Private methods
void IO::setupEncoder() {
    // Enable pullups if requested
    if (pullupsEnabled) {
        pinMode(encoderPinA, INPUT_PULLUP);
        pinMode(encoderPinB, INPUT_PULLUP);
    } else {
        pinMode(encoderPinA, INPUT);
        pinMode(encoderPinB, INPUT);
    }

    // Initialize ESP32Encoder
    ESP32Encoder::useInternalWeakPullResistors = pullupsEnabled ? puType::up : puType::none;
    encoder.attachFullQuad(encoderPinA, encoderPinB);
    encoder.clearCount();

    lastEncoderPosition = 0;
    encoderDelta = 0;
}

void IO::setupButton() {
    if (pullupsEnabled) {
        pinMode(buttonPin, INPUT_PULLUP);
    } else {
        pinMode(buttonPin, INPUT);
    }

    // Initialize button state
    buttonState = readButtonRaw();
    lastButtonState = buttonState;
    buttonPressed = false;
    buttonReleased = false;
    lastButtonChange = millis();
}

void IO::updateEncoder() {
    long currentPosition = encoder.getCount();

    // Apply direction reversal if needed
    if (encoderReversed) {
        currentPosition = -currentPosition;
    }

    // Calculate delta
    int delta = (int)(currentPosition - lastEncoderPosition);

    if (delta != 0) {
        encoderDelta = delta;
        lastEncoderPosition = currentPosition;
        newInputAvailable = true;

        // Call callback if set
        if (encoderCallback) {
            encoderCallback(delta);
        }
    }
}

void IO::updateButton() {
    unsigned long currentTime = millis();
    bool currentButtonReading = readButtonRaw();

    // Debouncing logic
    if (currentButtonReading != lastButtonState) {
        lastButtonChange = currentTime;
    }

    if ((currentTime - lastButtonChange) > buttonDebounceTime) {
        // Button state has stabilized
        if (currentButtonReading != buttonState) {
            buttonState = currentButtonReading;
            newInputAvailable = true;

            if (buttonState) {
                buttonPressed = true;
            } else {
                buttonReleased = true;
            }

            // Call callback if set
            if (buttonCallback) {
                buttonCallback(buttonState);
            }
        }
    }

    lastButtonState = currentButtonReading;
}

bool IO::readButtonRaw() {
    // Button is active LOW when using pullups
    if (pullupsEnabled) {
        return !digitalRead(buttonPin);
    } else {
        return digitalRead(buttonPin);
    }
}
