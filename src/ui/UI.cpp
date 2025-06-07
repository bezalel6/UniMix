#include "UI.hpp"
#include "../network/network.hpp"

// Initialize static member
UI* UI::instance = nullptr;

// Private constructor
UI::UI() : display(nullptr), initialized(false), currentScreen(SCREEN_HELLO_WORLD), currentRotation(1) {
    // Initialize the e-paper display
    display = new GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT>(
        GxEPD2_290_BS(/*CS=5*/ 5, /*DC=*/0, /*RES=*/2, /*BUSY=*/15));
}

// Destructor
UI::~UI() {
    if (initialized) {
        shutdown();
    }
    if (display) {
        delete display;
        display = nullptr;
    }
}

// Get singleton instance
UI& UI::getInstance() {
    if (instance == nullptr) {
        instance = new UI();
    }
    return *instance;
}

// Check if instance exists
bool UI::hasInstance() {
    return instance != nullptr;
}

// Destroy the singleton instance
void UI::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

// Initialize the UI system
void UI::initialize() {
    if (!initialized && display) {
        initializeDisplay();
        initialized = true;
    }
}

// Shutdown the UI system
void UI::shutdown() {
    if (initialized && display) {
        hibernateDisplay();
        initialized = false;
    }
}

// Hibernate the display to save power
void UI::hibernateDisplay() {
    if (display) {
        display->hibernate();
    }
}

// Wake the display from hibernation
void UI::wakeDisplay() {
    if (display) {
        display->init(115200, true, 50, false);
    }
}

// Screen display methods
void UI::showHelloWorld() {
    if (!initialized || !display) return;

    const char HelloWorld[] = "Hello World!";
    const char HelloWeACtStudio[] = "WeAct Studio";

    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    display->setTextColor(GxEPD_BLACK);

    int16_t tbx, tby;
    uint16_t tbw, tbh;

    // Center "Hello World!"
    display->getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display->width() - tbw) / 2) - tbx;
    uint16_t y = ((display->height() - tbh) / 2) - tby;

    display->setFullWindow();
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        display->setCursor(x, y - tbh);
        display->print(HelloWorld);

        // Center "WeAct Studio" below
        display->setTextColor(display->epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
        display->getTextBounds(HelloWeACtStudio, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = ((display->width() - tbw) / 2) - tbx;
        display->setCursor(x, y + tbh);
        display->print(HelloWeACtStudio);
    } while (display->nextPage());

    currentScreen = SCREEN_HELLO_WORLD;
}

void UI::showFullScreenPartialMode() {
    if (!initialized || !display) return;

    const char fullscreen[] = "full screen update";
    const char fpm[] = "fast partial mode";
    const char spm[] = "slow partial mode";
    const char npm[] = "no partial mode";
    const char HelloWorld[] = "Hello World!";

    display->setPartialWindow(0, 0, display->width(), display->height());
    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    if (display->epd2.WIDTH < 104) display->setFont(0);
    display->setTextColor(GxEPD_BLACK);

    const char* updatemode;
    if (display->epd2.hasFastPartialUpdate) {
        updatemode = fpm;
    } else if (display->epd2.hasPartialUpdate) {
        updatemode = spm;
    } else {
        updatemode = npm;
    }

    int16_t tbx, tby;
    uint16_t tbw, tbh;

    // Center update text
    display->getTextBounds(fullscreen, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t utx = ((display->width() - tbw) / 2) - tbx;
    uint16_t uty = ((display->height() / 4) - tbh / 2) - tby;

    // Center update mode
    display->getTextBounds(updatemode, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t umx = ((display->width() - tbw) / 2) - tbx;
    uint16_t umy = ((display->height() * 3 / 4) - tbh / 2) - tby;

    // Center HelloWorld
    display->getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t hwx = ((display->width() - tbw) / 2) - tbx;
    uint16_t hwy = ((display->height() - tbh) / 2) - tby;

    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        display->setCursor(hwx, hwy);
        display->print(HelloWorld);
        display->setCursor(utx, uty);
        display->print(fullscreen);
        display->setCursor(umx, umy);
        display->print(updatemode);
    } while (display->nextPage());

    currentScreen = SCREEN_PARTIAL_MODE;
}

void UI::showPartialUpdateDemo() {
    if (!initialized || !display) return;

    // Show background first
    showHelloWorld();
    delay(1000);

    // Demo partial updates
    showPartialUpdateBox();
    animatePartialUpdates();

    currentScreen = SCREEN_PARTIAL_DEMO;
}

void UI::showMainMenu() {
    if (!initialized || !display) return;

    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    display->setTextColor(GxEPD_BLACK);
    display->setFullWindow();

    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        displayTextCenteredAt("MAIN MENU", display->height() / 8);
        displayTextCenteredAt("1. Hello World", display->height() / 4);
        displayTextCenteredAt("2. Partial Mode", display->height() * 3 / 8);
        displayTextCenteredAt("3. Partial Demo", display->height() / 2);
        displayTextCenteredAt("4. Settings", display->height() * 5 / 8);
        displayTextCenteredAt("5. Status", display->height() * 3 / 4);
        displayTextCenteredAt("6. Network", display->height() * 7 / 8);
        displayTextCenteredAt("7. Progress Bar", display->height() * 8 / 9);
    } while (display->nextPage());

    currentScreen = SCREEN_MAIN_MENU;
}

void UI::showSettingsScreen() {
    if (!initialized || !display) return;

    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    display->setTextColor(GxEPD_BLACK);
    display->setFullWindow();

    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        displayTextCenteredAt("SETTINGS", display->height() / 4);
        displayTextCenteredAt(("Rotation: " + String(currentRotation)).c_str(), display->height() / 2);
        displayTextCenteredAt("Display: E-Paper", display->height() * 3 / 4);
    } while (display->nextPage());

    currentScreen = SCREEN_SETTINGS;
}

void UI::showStatusScreen() {
    if (!initialized || !display) return;

    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    display->setTextColor(GxEPD_BLACK);
    display->setFullWindow();

    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        displayTextCenteredAt("STATUS", display->height() / 8);
        displayTextCenteredAt(("Width: " + String(display->width())).c_str(), display->height() / 4);
        displayTextCenteredAt(("Height: " + String(display->height())).c_str(), display->height() * 3 / 8);
        displayTextCenteredAt(("Partial: " + String(hasPartialUpdate() ? "YES" : "NO")).c_str(), display->height() / 2);
        displayTextCenteredAt(("Fast Partial: " + String(hasFastPartialUpdate() ? "YES" : "NO")).c_str(), display->height() * 5 / 8);

        // Display network status at bottom
        displayNetworkStatus(0, display->height() * 3 / 4, true);
    } while (display->nextPage());

    currentScreen = SCREEN_STATUS;
}

// Display utility methods
void UI::setRotation(uint16_t rotation) {
    currentRotation = rotation;
    if (display) {
        display->setRotation(rotation);
    }
}

void UI::clearScreen() {
    if (!initialized || !display) return;

    display->setFullWindow();
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
    } while (display->nextPage());
}

void UI::updateScreen() {
    if (!initialized || !display) return;

    switch (currentScreen) {
        case SCREEN_HELLO_WORLD:
            showHelloWorld();
            break;
        case SCREEN_PARTIAL_MODE:
            showFullScreenPartialMode();
            break;
        case SCREEN_PARTIAL_DEMO:
            showPartialUpdateDemo();
            break;
        case SCREEN_MAIN_MENU:
            showMainMenu();
            break;
        case SCREEN_SETTINGS:
            showSettingsScreen();
            break;
        case SCREEN_STATUS:
            showStatusScreen();
            break;
        case SCREEN_NETWORK:
            showNetworkScreen();
            break;
        case SCREEN_PROGRESS_BAR:
            showProgressBarScreen();
            break;
        default:
            showMainMenu();
            break;
    }
}

// Text display utilities
void UI::displayText(const char* text, uint16_t x, uint16_t y, bool centered) {
    if (!display) return;

    if (centered) {
        int16_t tbx, tby;
        uint16_t tbw, tbh;
        display->getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = x - (tbw / 2);
        y = y - (tbh / 2);
    }

    display->setCursor(x, y);
    display->print(text);
}

void UI::displayTextCentered(const char* text) {
    if (!display) return;

    uint16_t x, y;
    getCenteredTextPosition(text, &x, &y);
    display->setCursor(x, y);
    display->print(text);
}

void UI::displayTextCenteredAt(const char* text, uint16_t y) {
    if (!display) return;

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display->getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display->width() - tbw) / 2) - tbx;
    display->setCursor(x, y);
    display->print(text);

    // Add network status indicator in top-right corner (except on network screen)
    if (currentScreen != SCREEN_NETWORK) {
        displayNetworkIndicator();
    }
}

void UI::displayNetworkIndicator() {
    if (!display) return;

    Network& network = Network::getInstance();

    // Use a small font for the indicator
    display->setFont(0);  // Default small font

    // Position in top-right corner
    const char* indicator;
    switch (network.getStatus()) {
        case NetworkStatus::CONNECTED:
            indicator = "WiFi";
            break;
        case NetworkStatus::CONNECTING:
        case NetworkStatus::RECONNECTING:
            indicator = "...";
            break;
        case NetworkStatus::DISCONNECTED:
        case NetworkStatus::FAILED:
        default:
            indicator = "X";
            break;
    }

    // Calculate position (top-right corner)
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display->getTextBounds(indicator, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = display->width() - tbw - 5;  // 5 pixels from right edge
    uint16_t y = tbh + 5;                     // 5 pixels from top edge

    display->setCursor(x, y);
    display->print(indicator);

    // Restore the main font
    display->setFont(&FreeMonoBold9pt7b);
}

// Screen navigation
void UI::nextScreen() {
    currentScreen = (currentScreen + 1) % MAX_SCREENS;
    updateScreen();
}

void UI::previousScreen() {
    currentScreen = (currentScreen - 1 + MAX_SCREENS) % MAX_SCREENS;
    updateScreen();
}

void UI::setCurrentScreen(int screen) {
    if (screen >= 0 && screen < MAX_SCREENS) {
        currentScreen = screen;
        updateScreen();
    }
}

int UI::getCurrentScreen() const {
    return currentScreen;
}

// Display properties
uint16_t UI::getDisplayWidth() const {
    return display ? display->width() : 0;
}

uint16_t UI::getDisplayHeight() const {
    return display ? display->height() : 0;
}

bool UI::hasPartialUpdate() const {
    return display ? display->epd2.hasPartialUpdate : false;
}

bool UI::hasFastPartialUpdate() const {
    return display ? display->epd2.hasFastPartialUpdate : false;
}

// Private methods
void UI::setupDisplay() {
    // Display setup is handled in initializeDisplay
}

void UI::initializeDisplay() {
    if (display) {
        display->init(115200, true, 50, false);
    }
}

void UI::getTextBounds(const char* text, int16_t* x, int16_t* y, uint16_t* w, uint16_t* h) {
    if (display) {
        display->getTextBounds(text, 0, 0, x, y, w, h);
    }
}

void UI::getCenteredTextPosition(const char* text, uint16_t* x, uint16_t* y) {
    if (!display) return;

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display->getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
    *x = ((display->width() - tbw) / 2) - tbx;
    *y = ((display->height() - tbh) / 2) - tby;
}

void UI::showPartialUpdateBox() {
    if (!display) return;

    uint16_t box_x = 10;
    uint16_t box_y = 15;
    uint16_t box_w = 70;
    uint16_t box_h = 20;

    // Show where the update box is
    for (uint16_t r = 0; r < 4; r++) {
        display->setRotation(r);
        display->setPartialWindow(box_x, box_y, box_w, box_h);
        display->firstPage();
        do {
            display->fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
        } while (display->nextPage());
        delay(2000);
        display->firstPage();
        do {
            display->fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        } while (display->nextPage());
        delay(1000);
    }
}

void UI::animatePartialUpdates() {
    if (!display) return;

    uint16_t box_x = 10;
    uint16_t box_y = 15;
    uint16_t box_w = 70;
    uint16_t box_h = 20;
    uint16_t cursor_y = box_y + box_h - 6;
    if (display->epd2.WIDTH < 104) cursor_y = box_y + 6;

    float value = 13.95;
    uint16_t incr = display->epd2.hasFastPartialUpdate ? 1 : 3;

    display->setFont(&FreeMonoBold9pt7b);
    if (display->epd2.WIDTH < 104) display->setFont(0);
    display->setTextColor(GxEPD_BLACK);

    // Show updates in the update box
    for (uint16_t r = 0; r < 4; r++) {
        display->setRotation(r);
        display->setPartialWindow(box_x, box_y, box_w, box_h);
        for (uint16_t i = 1; i <= 10; i += incr) {
            display->firstPage();
            do {
                display->fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
                display->setCursor(box_x, cursor_y);
                display->print(value * i, 2);
            } while (display->nextPage());
            delay(500);
        }
        delay(1000);
        display->firstPage();
        do {
            display->fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        } while (display->nextPage());
        delay(1000);
    }
}

void UI::showNetworkScreen() {
    if (!initialized || !display) return;

    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    display->setTextColor(GxEPD_BLACK);
    display->setFullWindow();

    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        displayTextCenteredAt("NETWORK", display->height() / 8);

        // Display detailed network status
        displayNetworkStatus(0, display->height() / 4, false);
    } while (display->nextPage());

    currentScreen = SCREEN_NETWORK;
}

void UI::displayNetworkStatus(uint16_t x, uint16_t y, bool compact) {
    if (!display) return;

    Network& network = Network::getInstance();

    if (compact) {
        // Compact display for status screen
        String status = "Net: " + String(network.getStatusString());
        displayTextCenteredAt(status.c_str(), y);

        if (network.isConnected()) {
            String ip = "IP: " + network.getLocalIP();
            displayTextCenteredAt(ip.c_str(), y + 20);
        }
    } else {
        // Detailed display for network screen
        uint16_t lineHeight = 25;
        uint16_t currentY = y;

        // Status
        String status = "Status: " + String(network.getStatusString());
        displayTextCenteredAt(status.c_str(), currentY);
        currentY += lineHeight;

        // SSID
        String ssid = "SSID: " + network.getSSID();
        displayTextCenteredAt(ssid.c_str(), currentY);
        currentY += lineHeight;

        if (network.isConnected()) {
            // IP Address
            String ip = "IP: " + network.getLocalIP();
            displayTextCenteredAt(ip.c_str(), currentY);
            currentY += lineHeight;

            // Signal strength
            String rssi = "RSSI: " + String(network.getRSSI()) + " dBm";
            displayTextCenteredAt(rssi.c_str(), currentY);
            currentY += lineHeight;

            // Connection time
            unsigned long connTime = network.getConnectedTime() / 1000;
            String uptime = "Up: " + String(connTime) + "s";
            displayTextCenteredAt(uptime.c_str(), currentY);
        } else {
            // Show reconnection attempts if disconnected
            if (network.getReconnectAttempts() > 0) {
                String attempts = "Attempts: " + String(network.getReconnectAttempts());
                displayTextCenteredAt(attempts.c_str(), currentY);
            }
        }
    }
}

void UI::showProgressBarScreen() {
    if (!initialized || !display) return;

    display->setRotation(currentRotation);
    display->setFont(&FreeMonoBold9pt7b);
    display->setTextColor(GxEPD_BLACK);
    display->setFullWindow();

    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);

        // Title
        displayTextCenteredAt("PROGRESS CONTROL", display->height() / 8);

        // Instructions
        display->setFont(0);  // Smaller font for instructions
        displayTextCenteredAt("Turn encoder to adjust", display->height() / 4);
        displayTextCenteredAt("Press button to reset", display->height() / 4 + 15);

        // Progress bar area (will be updated separately)
        display->setFont(&FreeMonoBold9pt7b);  // Restore main font
        displayTextCenteredAt("Progress:", display->height() / 2);

        // Draw initial progress bar at 50%
        uint16_t barWidth = display->width() - 40;
        uint16_t barHeight = 30;
        uint16_t barX = (display->width() - barWidth) / 2;
        uint16_t barY = display->height() * 5 / 8;

        drawProgressBar(50, barX, barY, barWidth, barHeight);

    } while (display->nextPage());

    currentScreen = SCREEN_PROGRESS_BAR;
}

void UI::updateProgressBar(int value, bool forceFullUpdate) {
    if (!initialized || !display || currentScreen != SCREEN_PROGRESS_BAR) return;

    // Progress bar dimensions
    uint16_t barWidth = display->width() - 40;
    uint16_t barHeight = 30;
    uint16_t barX = (display->width() - barWidth) / 2;
    uint16_t barY = display->height() * 5 / 8;

    // Value text area
    uint16_t valueY = barY + barHeight + 25;

    if (forceFullUpdate) {
        // Full screen update
        display->setFullWindow();
        display->firstPage();
        do {
            display->fillScreen(GxEPD_WHITE);

            // Redraw the entire screen
            display->setFont(&FreeMonoBold9pt7b);
            display->setTextColor(GxEPD_BLACK);

            displayTextCenteredAt("PROGRESS CONTROL", display->height() / 8);

            display->setFont(0);
            displayTextCenteredAt("Turn encoder to adjust", display->height() / 4);
            displayTextCenteredAt("Press button to reset", display->height() / 4 + 15);

            display->setFont(&FreeMonoBold9pt7b);
            displayTextCenteredAt("Progress:", display->height() / 2);

            drawProgressBar(value, barX, barY, barWidth, barHeight);

            // Display percentage
            String valueStr = String(value) + "%";
            displayTextCenteredAt(valueStr.c_str(), valueY);

        } while (display->nextPage());
    } else {
        // Partial update for just the progress bar and value
        uint16_t updateX = barX - 5;
        uint16_t updateY = barY - 5;
        uint16_t updateW = barWidth + 10;
        uint16_t updateH = barHeight + 40;  // Include space for percentage text

        display->setPartialWindow(updateX, updateY, updateW, updateH);
        display->firstPage();
        do {
            // Clear the update area
            display->fillRect(updateX, updateY, updateW, updateH, GxEPD_WHITE);

            // Redraw progress bar
            drawProgressBar(value, barX, barY, barWidth, barHeight);

            // Display percentage
            display->setFont(&FreeMonoBold9pt7b);
            display->setTextColor(GxEPD_BLACK);
            String valueStr = String(value) + "%";
            displayTextCenteredAt(valueStr.c_str(), valueY);

        } while (display->nextPage());
    }
}

void UI::drawProgressBar(int value, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    if (!display) return;

    // Clamp value to 0-100
    if (value < 0) value = 0;
    if (value > 100) value = 100;

    // Draw outer border
    display->drawRect(x, y, width, height, GxEPD_BLACK);

    // Calculate filled width
    uint16_t fillWidth = ((width - 4) * value) / 100;  // -4 for 2px border on each side

    // Draw filled portion
    if (fillWidth > 0) {
        display->fillRect(x + 2, y + 2, fillWidth, height - 4, GxEPD_BLACK);
    }

    // Draw empty portion
    if (fillWidth < width - 4) {
        display->fillRect(x + 2 + fillWidth, y + 2, width - 4 - fillWidth, height - 4, GxEPD_WHITE);
    }
}
