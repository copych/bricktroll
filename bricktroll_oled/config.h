#pragma once

#define DEBUG_TO_SERIAL
#define DEBUG_TO_DISPLAY

// ================================================================== JOYSTICKS ======================================================================
#define JOY_LEFT_X_GPIO 34
#define JOY_LEFT_Y_GPIO 35

#define JOY_RIGHT_X_GPIO 25
#define JOY_RIGHT_Y_GPIO 26

// at least one of two switches should be on this list of RTC GPIOs: 0, 2, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33, 34, 35, 36, 37, 38, 39 
// otherwise you won't be able to wake up the device after it goes to sleep
#define JOY_LEFT_SW_GPIO 32
#define JOY_RIGHT_SW_GPIO 33

#define ACTIVE_HIGH

// ================================================================== DISPLAY ======================================================================
#define DISPLAY_SSD1306

#define DISPLAY_SCL 22
#define DISPLAY_SDA 21

#define SMALL_FONT  u8g2_font_ncenR08_tr
#define MEDIUM_FONT u8g2_font_ncenR10_tr
#define BIG_FONT    u8g2_font_ncenB10_tr
#define HUGE_FONT   u8g2_font_ncenB12_tr

// #define DISPLAY_ROTATE_180

// ================================================================== YOU SHOULDN't NEED TO CHANGE ANYTHING BELOW THIS LINE  ======================================================================
/* 
 char* formatStr(...) {
  va_list ap;
  char targ[126];
  int n = sprintf(targ, ap);  
  return &targ;
}
*/
#ifndef DEB
  #ifdef DEBUG_TO_SERIAL
    #define DEB(...) Serial.print(__VA_ARGS__) 
    #define DEBF(...) Serial.printf(__VA_ARGS__) 
    #define DEBUG(...) Serial.println(__VA_ARGS__)
    /*
  #elif defined DEBUG_TO_DISPLAY
    #define DEB(...) u8g2.print(__VA_ARGS__) 
    #define DEBF(...) u8g2.print(formatStr(__VA_ARGS__)) 
    #define DEBUG(...) u8g2.print(__VA_ARGS__); u8g2.print("\r\n");
    */
  #else
    #define DEB(...)
    #define DEBF(...)
    #define DEBUG(...)
  #endif
#endif

#if defined DISPLAY_ROTATE_180
#define U8_ROTATE U8G2_R2
#else
#define U8_ROTATE U8G2_R0
#endif
