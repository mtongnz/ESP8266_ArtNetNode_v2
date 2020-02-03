/****  APP SETTINGS  ****/
#define RESET_ENABLE        // If enabled the Settings of the device can be reset by pulling the signal at pin SETTINGS_RESET low on startup
#define DEBUG_ENABLE		// Only using DMX Port A because B is on the same serial as the Serial.println


/****  ArtNet Definitions  ****/
#define ARTNET_OEM 0x0123    // Artnet OEM Code
#define ESTA_MAN 0x08DD      // ESTA Manufacturer Code
#define ESTA_DEV 0xEE000000  // RDM Device ID (used with Man Code to make 48bit UID)


/****  Ports  ****/
// Reset Pin (if NO_RESET is defined the SETTINGS_RESET wont be used)
#define SETTINGS_RESET 14   // Wemos D5

// DMX Direction pin for RS485 driver (send / receive)
#define DMX_DIR_A 5     // Wemos D1
#define DMX_DIR_B 16    // Wemos D0

// DMX Transmit pin for RS485 driver
#define DMX_TX_A 1
#define DMX_TX_B 2

// Device Status LED (RGB LED but only R and G used)
#define STATUS_LED_S_R 4    // Wemos D2
#define STATUS_LED_S_G 0    // Wemos D3

// DMX Activity LEDs (e.g. blue LED)
#define DMX_ACT_LED_A 12    // Wemos D6
#define DMX_ACT_LED_B 13    // Wemos D7


/**** LED Settings  ****/
#define STATUS_LED_BRIGHTNESS 100     // Brightness of Status LED when HIGH (0-1023)
#define DMX_ACT_LED_BRIGHTNESS 100    // Brightness of DMX LED when HIGH    (0-1023)
