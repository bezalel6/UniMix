#include <Arduino.h>
#include "io/IO.hpp"
#include "ui/UI.hpp"

// Progress bar state
int targetProgressValue = 50;        // Target value from encoder
float currentProgressValue = 50.0f;  // Current animated value
bool needsDisplayUpdate = true;
unsigned long lastDisplayUpdate = 0;
unsigned long lastAnimationUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 100;   // Update display every 100ms for better responsiveness
const unsigned long ANIMATION_UPDATE_INTERVAL = 16;  // Update animation every 16ms (60 FPS)
const float ANIMATION_SPEED = 12.0f;                 // Progress units per animation frame (much faster)

// Forward declarations
void updateDisplay();
void updateAnimation();

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Starting Progress Bar Controller with E-Paper Display");

    // Initialize UI system first
    UI& ui = UI::getInstance();
    ui.initialize();

    // Get the IO manager instance
    IO& io = IO::getInstance();

    // Configure single rotary encoder
    RotaryEncoder::Config encoderConfig;
    encoderConfig.pinA = 32;
    encoderConfig.pinB = 33;
    encoderConfig.buttonPin = 25;
    encoderConfig.hasButton = true;
    encoderConfig.enablePullups = true;
    encoderConfig.reversed = false;  // Change this if encoder direction is wrong

    RotaryEncoder* encoder = io.addRotaryEncoder("progress_encoder", encoderConfig);

    // Set up encoder callbacks
    if (encoder) {
        encoder->setEncoderCallback([](int delta) {
            // Update target progress value with encoder movement
            targetProgressValue += delta;

            // Clamp to 0-100 range
            if (targetProgressValue < 0) targetProgressValue = 0;
            if (targetProgressValue > 100) targetProgressValue = 100;

            Serial.printf("Target Progress: %d%% (delta: %d)\n", targetProgressValue, delta);
        });

        encoder->setButtonCallback([](bool pressed) {
            if (pressed) {
                // Reset to 50% when button is pressed
                targetProgressValue = 50;
                Serial.println("Target progress reset to 50%");
            }
        });
    }

    // Initialize the IO system
    io.initialize();

    // Set to progress bar screen
    ui.setCurrentScreen(7);  // SCREEN_PROGRESS_BAR

    Serial.println("Progress bar controller initialized");
    Serial.println("- Turn encoder to adjust progress (0-100%)");
    Serial.println("- Press encoder button to reset to 50%");
    Serial.println("- Progress displayed on e-paper screen");

    // Initial display update
    updateDisplay();
}

void loop() {
    // Update all input devices
    IO::getInstance().update();

    unsigned long currentTime = millis();

    // Update animation at regular intervals
    if ((currentTime - lastAnimationUpdate) >= ANIMATION_UPDATE_INTERVAL) {
        updateAnimation();
        lastAnimationUpdate = currentTime;
    }

    // Update display if needed and enough time has passed
    if (needsDisplayUpdate && (currentTime - lastDisplayUpdate) >= DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        needsDisplayUpdate = false;
        lastDisplayUpdate = currentTime;
    }

    delay(1);  // Small delay for stability
}

void updateAnimation() {
    // Smoothly animate current value toward target value
    float difference = targetProgressValue - currentProgressValue;

    if (abs(difference) > 0.1f) {
        // Use adaptive speed - faster for larger differences
        float adaptiveSpeed = ANIMATION_SPEED;

        // Scale speed based on distance (faster for big jumps)
        float distance = abs(difference);
        if (distance > 20) {
            adaptiveSpeed *= 2.0f;  // Double speed for large changes
        } else if (distance > 10) {
            adaptiveSpeed *= 1.5f;  // 1.5x speed for medium changes
        }

        // Calculate movement direction and amount
        float direction = (difference > 0) ? 1.0f : -1.0f;
        float movement = min(adaptiveSpeed, abs(difference)) * direction;

        // Move toward target
        currentProgressValue += movement;

        // Snap to target if very close (prevents endless tiny movements)
        if (abs(targetProgressValue - currentProgressValue) < 0.5f) {
            currentProgressValue = targetProgressValue;
        }

        // Flag for display update
        needsDisplayUpdate = true;

        // Reduced debug output (only for significant changes)
        if (abs(movement) > 1.0f) {
            Serial.printf("Animating: %.1f%% -> %d%% (speed: %.1f)\n",
                          currentProgressValue, targetProgressValue, adaptiveSpeed);
        }
    }
}

void updateDisplay() {
    // Update the e-paper display progress bar
    UI& ui = UI::getInstance();

    // Use partial update for fast refresh (if supported)
    // Force full update every 30 updates to prevent ghosting (increased frequency)
    static int updateCount = 0;
    static int lastDisplayedValue = -1;
    updateCount++;

    // Convert float to int for display
    int displayValue = (int)round(currentProgressValue);

    // Only update if value actually changed (reduces unnecessary updates)
    if (displayValue != lastDisplayedValue) {
        bool forceFullUpdate = (updateCount % 30 == 0);
        ui.updateProgressBar(displayValue, forceFullUpdate);
        lastDisplayedValue = displayValue;

        // Reduced debug output
        Serial.printf("Display: %d%%\n", displayValue);
    }
}