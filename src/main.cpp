#include "ui/UI.hpp"
#include "io/IO.hpp"
#include "network/network.hpp"

// ESP32 CS(SS)=5,SCL(SCK)=18,SDA(MOSI)=23,BUSY=15,RES(RST)=2,DC=0

// 1.54'' EPD Module
// GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=5*/ 5, /*DC=*/ 0, /*RES=*/ 2, /*BUSY=*/ 15)); // GDEH0154D67 200x200, SSD1681

// 2.13'' EPD Module
// GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ 5, /*DC=*/0, /*RES=*/2, /*BUSY=*/15));  // DEPG0213BN 122x250, SSD1680
// GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ 5, /*DC=*/ 0, /*RES=*/ 2, /*BUSY=*/ 15)); // GDEY0213Z98 122x250, SSD1680

// 2.9'' EPD Module
// GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(/*CS=5*/ 5, /*DC=*/0, /*RES=*/2, /*BUSY=*/15));  // DEPG0290BS 128x296, SSD1680
// GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(/*CS=5*/ 5, /*DC=*/ 0, /*RES=*/ 2, /*BUSY=*/ 15)); // GDEM029C90 128x296, SSD1680

// 4.2'' EPD Module
// GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=5*/ 5, /*DC=*/ 0, /*RES=*/ 2, /*BUSY=*/ 15)); // 400x300, SSD1683

// Global variables for state management
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 50;  // Update every 50ms

void setup() {
    Serial.begin(115200);

    // Initialize UI, IO, and Network singletons
    UI& ui = UI::getInstance();
    IO& io = IO::getInstance();
    Network& network = Network::getInstance();

    // Configure IO pins (optional - uses defaults if not called)
    // io.setEncoderPins(32, 33, 25); // A, B, Button pins

    // Initialize systems
    ui.initialize();
    io.initialize();
    network.initialize();

    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    network.connect();

    Serial.println("UniMix System Initialized");
    Serial.println("Use rotary encoder to navigate:");
    Serial.println("- Rotate: Change screens");
    Serial.println("- Press: Trigger screen action");

    // Show initial screen
    ui.showMainMenu();

    // Small delay to let everything settle
    delay(1000);
}

void handleButtonPress(), handleUserInput();
void loop() {
    // Update IO and Network state
    IO& io = IO::getInstance();
    Network& network = Network::getInstance();

    io.update();
    network.update();

    // Check for input and update UI accordingly
    handleUserInput();

    // Add small delay to prevent overwhelming the system
    delay(10);
}
void handleUserInput() {
    UI& ui = UI::getInstance();
    IO& io = IO::getInstance();

    // Handle rotary encoder rotation for navigation
    int encoderDelta = io.getEncoderDelta();
    if (encoderDelta != 0) {
        Serial.print("Encoder rotated: ");
        Serial.println(encoderDelta);

        if (encoderDelta > 0) {
            // Clockwise rotation - next screen
            ui.nextScreen();
            Serial.println("Next screen");
        } else {
            // Counter-clockwise rotation - previous screen
            ui.previousScreen();
            Serial.println("Previous screen");
        }

        // Clear the input flags after handling
        io.clearInputFlags();
    }

    // Handle button press for screen actions
    if (io.wasEncoderButtonPressed()) {
        Serial.println("Button pressed!");
        handleButtonPress();
    }

    // Handle button release
    if (io.wasEncoderButtonReleased()) {
        Serial.println("Button released!");
    }
}

void handleButtonPress() {
    UI& ui = UI::getInstance();
    int currentScreen = ui.getCurrentScreen();

    Serial.print("Button action on screen: ");
    Serial.println(currentScreen);

    switch (currentScreen) {
        case 0:  // Hello World screen
            Serial.println("Action: Showing Hello World demo");
            ui.showHelloWorld();
            break;

        case 1:  // Partial Mode screen
            Serial.println("Action: Showing Partial Mode demo");
            ui.showFullScreenPartialMode();
            break;

        case 2:  // Partial Demo screen
            Serial.println("Action: Starting Partial Update demo");
            if (ui.hasFastPartialUpdate()) {
                ui.showPartialUpdateDemo();
            } else {
                Serial.println("Fast partial update not available");
            }
            break;

        case 3:  // Main Menu screen
            Serial.println("Action: Refreshing main menu");
            ui.showMainMenu();
            break;

        case 4:  // Settings screen
            Serial.println("Action: Cycling rotation");
            // Cycle through rotation settings (0, 1, 2, 3)
            static int rotation = 1;
            rotation = (rotation + 1) % 4;
            ui.setRotation(rotation);
            ui.showSettingsScreen();  // Refresh settings display
            break;

        case 5:  // Status screen
            Serial.println("Action: Refreshing status");
            ui.showStatusScreen();
            break;

        case 6:  // Network screen
            Serial.println("Action: Refreshing network status");
            ui.showNetworkScreen();
            break;

        default:
            Serial.println("Unknown screen action");
            ui.showMainMenu();
            break;
    }
}

// Example callback functions (optional - can be set in setup)
void onEncoderRotated(int delta) {
    Serial.print("Encoder callback: ");
    Serial.println(delta);
}

void onButtonStateChanged(bool pressed) {
    Serial.print("Button callback: ");
    Serial.println(pressed ? "PRESSED" : "RELEASED");
}