#include <Arduino.h>
#include "../src/io/IO.hpp"

// Example showing how to use the new dynamic IO system
void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Starting IO Manager Example");

    // Get the IO manager instance
    IO& io = IO::getInstance();

    // Add a main rotary encoder with button
    RotaryEncoder::Config mainEncoderConfig;
    mainEncoderConfig.pinA = 32;
    mainEncoderConfig.pinB = 33;
    mainEncoderConfig.buttonPin = 25;
    mainEncoderConfig.hasButton = true;
    mainEncoderConfig.enablePullups = true;

    RotaryEncoder* mainEncoder = io.addRotaryEncoder("main_encoder", mainEncoderConfig);

    // Add a secondary encoder without button for volume control
    RotaryEncoder::Config volumeEncoderConfig;
    volumeEncoderConfig.pinA = 26;
    volumeEncoderConfig.pinB = 27;
    volumeEncoderConfig.hasButton = false;
    volumeEncoderConfig.enablePullups = true;

    RotaryEncoder* volumeEncoder = io.addRotaryEncoder("volume_encoder", volumeEncoderConfig);

    // Add standalone buttons
    Button::Config button1Config;
    button1Config.pin = 14;
    button1Config.enablePullup = true;
    button1Config.activeLow = true;

    Button* button1 = io.addButton("button1", button1Config);

    Button::Config button2Config;
    button2Config.pin = 12;
    button2Config.enablePullup = true;
    button2Config.activeLow = true;

    Button* button2 = io.addButton("button2", button2Config);

    // Set up callbacks for specific devices
    if (mainEncoder) {
        mainEncoder->setEncoderCallback([](int delta) {
            Serial.printf("Main Encoder moved: %d\n", delta);
        });

        mainEncoder->setButtonCallback([](bool pressed) {
            Serial.printf("Main Encoder button %s\n", pressed ? "pressed" : "released");
        });
    }

    if (volumeEncoder) {
        volumeEncoder->setEncoderCallback([](int delta) {
            Serial.printf("Volume changed: %d\n", delta);
        });
    }

    if (button1) {
        button1->setCallback([](bool pressed) {
            Serial.printf("Button 1 %s\n", pressed ? "pressed" : "released");
        });
    }

    if (button2) {
        button2->setCallback([](bool pressed) {
            Serial.printf("Button 2 %s\n", pressed ? "pressed" : "released");
        });
    }

    // Set up global input callback
    io.setGlobalInputCallback([](const String& deviceId, InputDevice::DeviceType type) {
        Serial.printf("Input detected from device: %s (type: %d)\n",
                      deviceId.c_str(), static_cast<int>(type));
    });

    // Initialize the IO system
    io.initialize();

    Serial.println("IO system initialized with multiple devices:");
    auto deviceIds = io.getDeviceIds();
    for (const String& id : deviceIds) {
        Serial.printf("- Device: %s\n", id.c_str());
    }
}

void loop() {
    // Update all input devices
    IO::getInstance().update();

    // Example of specific device access
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 1000) {  // Check every second
        lastCheck = millis();

        // Access specific devices by name
        RotaryEncoder* mainEncoder = IO::getInstance().getRotaryEncoder("main_encoder");
        if (mainEncoder) {
            Serial.printf("Main encoder position: %ld\n", mainEncoder->getPosition());
        }

        RotaryEncoder* volumeEncoder = IO::getInstance().getRotaryEncoder("volume_encoder");
        if (volumeEncoder) {
            Serial.printf("Volume encoder position: %ld\n", volumeEncoder->getPosition());
        }

        // Check button states
        Button* button1 = IO::getInstance().getButton("button1");
        if (button1 && button1->isPressed()) {
            Serial.printf("Button 1 held for %lu ms\n", button1->getPressedDuration());
        }
    }

    // Example of using the generic interface
    if (IO::getInstance().hasNewInput()) {
        Serial.println("Some input device has new input!");

        // Clear all input flags at once
        // IO::getInstance().clearAllInputFlags();
    }

    delay(10);  // Small delay for stability
}

// Alternative example showing dynamic device management
void alternativeExample() {
    IO& io = IO::getInstance();

    Serial.println("\n=== Dynamic Device Management Example ===");

    // Add devices dynamically using factory methods
    RotaryEncoder::Config dynamicEncoderConfig;
    dynamicEncoderConfig.pinA = 18;
    dynamicEncoderConfig.pinB = 19;
    dynamicEncoderConfig.hasButton = false;

    Button::Config dynamicButtonConfig;
    dynamicButtonConfig.pin = 13;

    auto encoder1 = RotaryEncoder::create("dynamic_encoder", dynamicEncoderConfig);
    auto button3 = Button::create("dynamic_button", dynamicButtonConfig);

    // Add to manager
    io.addDevice(std::move(encoder1));
    io.addDevice(std::move(button3));

    // Later, remove a device
    if (io.hasDevice("dynamic_button")) {
        Serial.println("Removing dynamic button");
        io.removeDevice("dynamic_button");
    }

    // Get all devices of a specific type - multiple ways

    // Method 1: Template method (generic)
    auto allEncoders = io.getDevicesOfType<RotaryEncoder>();
    Serial.printf("Found %d rotary encoders (template method)\n", allEncoders.size());

    auto allButtons = io.getDevicesOfType<Button>();
    Serial.printf("Found %d buttons (template method)\n", allButtons.size());

    // Method 2: Type-specific methods (simpler, no templates)
    auto encoders = io.getRotaryEncoders();
    Serial.printf("Found %d rotary encoders (direct method)\n", encoders.size());

    auto buttons = io.getButtons();
    Serial.printf("Found %d buttons (direct method)\n", buttons.size());
}