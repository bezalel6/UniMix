#include "RotaryEncoder.hpp"

RotaryEncoder::RotaryEncoder(const String& deviceId, const Config& config)
    : TypedInputDevice<RotaryEncoder>(deviceId, DeviceType::ENCODER),
      config(config),
      lastPosition(0),
      delta(0),
      newEncoderInput(false),
      buttonState(false),
      lastButtonState(false),
      buttonPressed(false),
      buttonReleased(false),
      lastButtonChange(0),
      newButtonInput(false) {
}

RotaryEncoder::~RotaryEncoder() {
    if (initialized) {
        shutdown();
    }
}

bool RotaryEncoder::initialize() {
    if (initialized) return true;

    setupEncoder();
    if (config.hasButton) {
        setupButton();
    }

    initialized = true;
    return true;
}

void RotaryEncoder::shutdown() {
    if (initialized) {
        initialized = false;
    }
}

void RotaryEncoder::update() {
    if (!initialized) return;

    updateEncoder();
    if (config.hasButton) {
        updateButton();
    }
}

bool RotaryEncoder::hasNewInput() {
    return newEncoderInput || (config.hasButton && newButtonInput);
}

void RotaryEncoder::clearInputFlags() {
    newEncoderInput = false;
    newButtonInput = false;
    buttonPressed = false;
    buttonReleased = false;
    delta = 0;
}

long RotaryEncoder::getPosition() const {
    if (!initialized) return 0;

    // Need to cast away const for ESP32Encoder API
    long position = const_cast<ESP32Encoder&>(encoder).getCount();
    return config.reversed ? -position : position;
}

int RotaryEncoder::getDelta() const {
    return delta;
}

void RotaryEncoder::resetPosition() {
    if (!initialized) return;

    encoder.clearCount();
    lastPosition = 0;
    delta = 0;
}

void RotaryEncoder::setPosition(long position) {
    if (!initialized) return;

    long actualPosition = config.reversed ? -position : position;
    encoder.setCount(actualPosition);
    lastPosition = actualPosition;
}

bool RotaryEncoder::isButtonPressed() const {
    return config.hasButton ? buttonState : false;
}

bool RotaryEncoder::wasButtonPressed() {
    if (!config.hasButton) return false;

    bool result = buttonPressed;
    buttonPressed = false;
    return result;
}

bool RotaryEncoder::wasButtonReleased() {
    if (!config.hasButton) return false;

    bool result = buttonReleased;
    buttonReleased = false;
    return result;
}

void RotaryEncoder::setReversed(bool reversed) {
    config.reversed = reversed;
}

void RotaryEncoder::setButtonDebounceTime(unsigned long debounceMs) {
    config.debounceTime = debounceMs;
}

void RotaryEncoder::setEncoderCallback(EncoderCallback callback) {
    encoderCallback = callback;
}

void RotaryEncoder::setButtonCallback(ButtonCallback callback) {
    buttonCallback = callback;
}

std::unique_ptr<RotaryEncoder> RotaryEncoder::create(const String& deviceId, const Config& config) {
    return std::unique_ptr<RotaryEncoder>(new RotaryEncoder(deviceId, config));
}

void RotaryEncoder::setupEncoder() {
    // Setup pins
    if (config.enablePullups) {
        pinMode(config.pinA, INPUT_PULLUP);
        pinMode(config.pinB, INPUT_PULLUP);
    } else {
        pinMode(config.pinA, INPUT);
        pinMode(config.pinB, INPUT);
    }

    // Initialize ESP32Encoder
    ESP32Encoder::useInternalWeakPullResistors = config.enablePullups ? puType::up : puType::none;
    encoder.attachFullQuad(config.pinA, config.pinB);
    encoder.clearCount();

    lastPosition = 0;
    delta = 0;
    newEncoderInput = false;
}

void RotaryEncoder::setupButton() {
    if (!config.hasButton) return;

    if (config.enablePullups) {
        pinMode(config.buttonPin, INPUT_PULLUP);
    } else {
        pinMode(config.buttonPin, INPUT);
    }

    // Initialize button state
    buttonState = readButtonRaw();
    lastButtonState = buttonState;
    buttonPressed = false;
    buttonReleased = false;
    lastButtonChange = millis();
    newButtonInput = false;
}

void RotaryEncoder::updateEncoder() {
    long currentPosition = encoder.getCount();

    // Apply direction reversal if needed
    if (config.reversed) {
        currentPosition = -currentPosition;
    }

    // Calculate delta
    int currentDelta = (int)(currentPosition - lastPosition);

    if (currentDelta != 0) {
        delta = currentDelta;
        lastPosition = currentPosition;
        newEncoderInput = true;

        // Call callback if set
        if (encoderCallback) {
            encoderCallback(currentDelta);
        }
    }
}

void RotaryEncoder::updateButton() {
    if (!config.hasButton) return;

    unsigned long currentTime = millis();
    bool currentReading = readButtonRaw();

    // Debouncing logic
    if (currentReading != lastButtonState) {
        lastButtonChange = currentTime;
    }

    if ((currentTime - lastButtonChange) > config.debounceTime) {
        // Button state has stabilized
        if (currentReading != buttonState) {
            buttonState = currentReading;
            newButtonInput = true;

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

    lastButtonState = currentReading;
}

bool RotaryEncoder::readButtonRaw() {
    if (!config.hasButton) return false;

    // Button is active LOW when using pullups
    if (config.enablePullups) {
        return !digitalRead(config.buttonPin);
    } else {
        return digitalRead(config.buttonPin);
    }
}