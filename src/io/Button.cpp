#include "Button.hpp"

Button::Button(const String& deviceId, const Config& config)
    : TypedInputDevice<Button>(deviceId, DeviceType::BUTTON),
      config(config),
      currentState(false),
      lastState(false),
      stateChanged(false),
      pressed(false),
      released(false),
      lastStateChange(0),
      lastPressed(0),
      lastReleased(0),
      newInput(false) {
}

Button::~Button() {
    if (initialized) {
        shutdown();
    }
}

bool Button::initialize() {
    if (initialized) return true;

    setupButton();
    initialized = true;
    return true;
}

void Button::shutdown() {
    if (initialized) {
        initialized = false;
    }
}

void Button::update() {
    if (!initialized) return;
    updateButton();
}

bool Button::hasNewInput() {
    return newInput;
}

void Button::clearInputFlags() {
    newInput = false;
    pressed = false;
    released = false;
    stateChanged = false;
}

bool Button::isPressed() const {
    return currentState;
}

bool Button::wasPressed() {
    bool result = pressed;
    pressed = false;
    return result;
}

bool Button::wasReleased() {
    bool result = released;
    released = false;
    return result;
}

unsigned long Button::getPressedDuration() const {
    if (!currentState) return 0;
    return millis() - lastPressed;
}

unsigned long Button::getReleasedDuration() const {
    if (currentState) return 0;
    return millis() - lastReleased;
}

void Button::setDebounceTime(unsigned long debounceMs) {
    config.debounceTime = debounceMs;
}

void Button::setCallback(ButtonCallback callback) {
    this->callback = callback;
}

std::unique_ptr<Button> Button::create(const String& deviceId, const Config& config) {
    return std::unique_ptr<Button>(new Button(deviceId, config));
}

void Button::setupButton() {
    if (config.enablePullup) {
        pinMode(config.pin, INPUT_PULLUP);
    } else {
        pinMode(config.pin, INPUT);
    }

    // Initialize button state
    currentState = readButtonRaw();
    lastState = currentState;
    lastStateChange = millis();

    if (currentState) {
        lastPressed = millis();
    } else {
        lastReleased = millis();
    }

    newInput = false;
}

void Button::updateButton() {
    unsigned long currentTime = millis();
    bool reading = readButtonRaw();

    // Debouncing logic
    if (reading != lastState) {
        lastStateChange = currentTime;
    }

    if ((currentTime - lastStateChange) > config.debounceTime) {
        // Button state has stabilized
        if (reading != currentState) {
            currentState = reading;
            stateChanged = true;
            newInput = true;

            if (currentState) {
                pressed = true;
                lastPressed = currentTime;
            } else {
                released = true;
                lastReleased = currentTime;
            }

            // Call callback if set
            if (callback) {
                callback(currentState);
            }
        }
    }

    lastState = reading;
}

bool Button::readButtonRaw() {
    bool reading = digitalRead(config.pin);

    // Apply active low logic if enabled
    if (config.activeLow) {
        reading = !reading;
    }

    return reading;
}