#include "ui/UI.hpp"

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

void setup() {
    // Get the UI singleton instance and initialize it
    UI& ui = UI::getInstance();
    ui.initialize();

    // Demonstrate all the screens from the original main.cpp

    // Show Hello World (equivalent to original helloWorld())
    ui.showHelloWorld();
    delay(3000);

    // Show Full Screen Partial Mode (equivalent to original helloFullScreenPartialMode())
    ui.showFullScreenPartialMode();
    delay(3000);

    // Show Partial Update Demo (equivalent to original showPartialUpdate())
    if (ui.hasFastPartialUpdate()) {
        ui.showPartialUpdateDemo();
    }

    // Show additional screens
    ui.showMainMenu();
    delay(3000);

    ui.showSettingsScreen();
    delay(3000);

    ui.showStatusScreen();
    delay(3000);

    // Demonstrate screen navigation
    ui.setCurrentScreen(0);  // Back to Hello World
    delay(2000);

    // Cycle through screens
    for (int i = 0; i < 6; i++) {
        ui.nextScreen();
        delay(2000);
    }

    // Hibernate the display to save power
    ui.hibernateDisplay();
}

void loop() {
    // In a real application, you might handle input here
    // and call ui.nextScreen(), ui.previousScreen(), etc.

    // Example of screen cycling in loop (uncomment if desired)
    /*
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000) { // Change screen every 10 seconds
        UI& ui = UI::getInstance();
        ui.nextScreen();
        lastUpdate = millis();
    }
    */
}