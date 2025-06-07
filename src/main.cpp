#include <Arduino.h>
#include "io/IO.hpp"
#include "ui/UI.hpp"

// Progress bar state
int progressValue = 50;  // Start at 50%
bool needsDisplayUpdate = true;
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 250;  // Update display every 250ms (e-paper is slower)

// Forward declarations
void updateDisplay();

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
            // Update progress value with encoder movement
            progressValue += delta;

            // Clamp to 0-100 range
            if (progressValue < 0) progressValue = 0;
            if (progressValue > 100) progressValue = 100;

            needsDisplayUpdate = true;

            Serial.printf("Progress: %d%% (delta: %d)\n", progressValue, delta);
        });

        encoder->setButtonCallback([](bool pressed) {
            if (pressed) {
                // Reset to 50% when button is pressed
                progressValue = 50;
                needsDisplayUpdate = true;
                Serial.println("Progress reset to 50%");
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

    // Update display if needed and enough time has passed
    unsigned long currentTime = millis();
    if (needsDisplayUpdate && (currentTime - lastDisplayUpdate) >= DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        needsDisplayUpdate = false;
        lastDisplayUpdate = currentTime;
    }

    delay(1);  // Small delay for stability
}

void updateDisplay() {
    // Update the e-paper display progress bar
    UI& ui = UI::getInstance();

    // Use partial update for fast refresh (if supported)
    // Force full update every 20 updates to prevent ghosting
    static int updateCount = 0;
    updateCount++;
    bool forceFullUpdate = (updateCount % 20 == 0);

    ui.updateProgressBar(progressValue, forceFullUpdate);

    // Also show in serial for debugging
    Serial.printf("Display updated: %d%%\n", progressValue);
}