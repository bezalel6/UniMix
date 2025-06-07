#pragma once
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

class UI {
   private:
    // Private constructor to prevent direct instantiation
    UI();

    // Static instance pointer
    static UI* instance;

    // Delete copy constructor and assignment operator
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;

   public:
    // Public destructor
    ~UI();

    // Static method to get the singleton instance
    static UI& getInstance();

    // Static method to check if instance exists
    static bool hasInstance();

    // Static method to destroy the instance
    static void destroyInstance();

    // UI lifecycle methods
    void initialize();
    void shutdown();
    void hibernateDisplay();
    void wakeDisplay();

    // Screen display methods
    void showHelloWorld();
    void showFullScreenPartialMode();
    void showPartialUpdateDemo();
    void showMainMenu();
    void showSettingsScreen();
    void showStatusScreen();
    void showNetworkScreen();
    void showProgressBarScreen();

    // Progress bar specific methods
    void updateProgressBar(int value, bool forceFullUpdate = false);
    void drawProgressBar(int value, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    // Display utility methods
    void setRotation(uint16_t rotation);
    void clearScreen();
    void updateScreen();

    // Text display utilities
    void displayText(const char* text, uint16_t x, uint16_t y, bool centered = false);
    void displayTextCentered(const char* text);
    void displayTextCenteredAt(const char* text, uint16_t y);
    void displayNetworkStatus(uint16_t x, uint16_t y, bool compact = false);
    void displayNetworkIndicator();

    // Screen navigation
    void nextScreen();
    void previousScreen();
    void setCurrentScreen(int screen);
    int getCurrentScreen() const;

    // Display properties
    uint16_t getDisplayWidth() const;
    uint16_t getDisplayHeight() const;
    bool hasPartialUpdate() const;
    bool hasFastPartialUpdate() const;

   private:
    // E-paper display instance (2.9" EPD Module)
    GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT>* display;

    // Internal state
    bool initialized;
    int currentScreen;
    uint16_t currentRotation;

    // Screen constants
    static const int SCREEN_HELLO_WORLD = 0;
    static const int SCREEN_PARTIAL_MODE = 1;
    static const int SCREEN_PARTIAL_DEMO = 2;
    static const int SCREEN_MAIN_MENU = 3;
    static const int SCREEN_SETTINGS = 4;
    static const int SCREEN_STATUS = 5;
    static const int SCREEN_NETWORK = 6;
    static const int SCREEN_PROGRESS_BAR = 7;
    static const int MAX_SCREENS = 8;

    // Internal methods
    void setupDisplay();
    void initializeDisplay();

    // Helper methods for text positioning
    void getTextBounds(const char* text, int16_t* x, int16_t* y, uint16_t* w, uint16_t* h);
    void getCenteredTextPosition(const char* text, uint16_t* x, uint16_t* y);

    // Partial update demo helpers
    void showPartialUpdateBox();
    void animatePartialUpdates();
};