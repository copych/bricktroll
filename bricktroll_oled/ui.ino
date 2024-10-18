#include "ui.h"
#define NUM_SWITCHES 2

// a simple script to show a text string during a given time
void textAnimation(const String& str, uint32_t msDelay , int32_t yShift , bool show ) {  
  u8g2.clearBuffer();
  // void U8G2::drawButtonUTF8(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags, u8g2_uint_t width, u8g2_uint_t padding_h, u8g2_uint_t padding_v, const char *text)
  u8g2.drawButtonUTF8(u8g2.getDisplayWidth()/2, u8g2.getDisplayHeight()/2 + 6 + yShift, U8G2_BTN_HCENTER|U8G2_BTN_BW0, u8g2.getDisplayWidth(),  0,  0, str.c_str() );
  if (show) {
    u8g2.sendBuffer();
    delay(msDelay);
  }
}


int Check_RTC() {
  const uint8_t RTC_pins[]{0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39}; 
  RTC_present = 0;
  for(int i = 0; i < NUM_SWITCHES; ++i) {
    sw_RTC[i] = false;
    for(int j = 0; j < (sizeof(RTC_pins)/sizeof(RTC_pins[0])); ++j) {
      if(joystick_button_gpio[i] == RTC_pins[j]) {
        sw_RTC[i] = true;
        RTC_present++;
        if (RTC_present == 1) {RTC_1st = i;}
        DEBUG("RTC pin: " + (String)(joystick_button_gpio[i]));
        break;
      }
    }    
  }
  DEBUG ("RTC pins present: " + (String)(RTC_present));
  return RTC_present;
}


// Stand-by mode with some fun
void ESP_off(){
  uint64_t bit_mask=0;
  String wake_buttons = "";
  int deep_sleep_pins;
  int k;
  deep_sleep_pins = Check_RTC();  // Find out if we have RTC pins assigned to buttons allowing deep sleep, otherwise we use light sleep
                                  //  CRT-off effect =) or something


  // Debug
  DEBUG("deep_sleep_pins = " + (String)(deep_sleep_pins));
  DEBUG("RTC_present = " + (String)(RTC_present));
    
  if (deep_sleep_pins > 0){
    u8g2.clear();
    u8g2.sendBuffer();
    u8g2.setFont(MEDIUM_FONT);
   
    //  Only GPIOs which have RTC functionality can be used for deep sleep: 0,2,4,12-15,25-27,32-39
  
    //  Future radio shutdown support here:
    //  esp_bluedroid_disable(); //gracefully shutdoun BT (and WiFi)
    //  esp_bt_controller_disable();
    //  esp_wifi_stop();

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    #ifdef ACTIVE_HIGH
      for (int i = 0; i < NUM_SWITCHES; i++) {
        if (sw_RTC[i]) {
          gpio_pullup_dis(static_cast <gpio_num_t> (joystick_button_gpio[i]));
          gpio_pulldown_en(static_cast <gpio_num_t> (joystick_button_gpio[i]));
          bit_mask += 1<<joystick_button_gpio[i];
          wake_buttons += (String)(i+1) + ",";
          k = i+1;
        }
      }
      if (deep_sleep_pins == NUM_SWITCHES) {
        wake_buttons = "Any button wakes";
      } else {
        if (deep_sleep_pins == 1) {
          wake_buttons = "Button " + (String)(k) + " wakes";
        } else {
          wake_buttons = "Buttons " + wake_buttons.substring(0, wake_buttons.length()-1) + " wake";
        }
      }
    #else
      gpio_pulldown_dis(static_cast <gpio_num_t> (joystick_button_gpio[RTC_1st]));
      gpio_pullup_en(static_cast <gpio_num_t> (joystick_button_gpio[RTC_1st]));
      bit_mask += 1<<joystick_button_gpio[RTC_1st];
      wake_buttons = "Button " + (String)(RTC_1st+1) + " wakes";
    #endif
    u8g2.setFont(MEDIUM_FONT);
    textAnimation(wake_buttons,3000);
    textAnimation("Deep sleep",1000);
    splash_off();

    DEBUG("Deep sleep"); 
    #ifdef ACTIVE_HIGH
      esp_sleep_enable_ext1_wakeup( bit_mask, ESP_EXT1_WAKEUP_ANY_HIGH); // wake up on RTC enabled GPIOs
    #else
      // if you use LOW as an active state, only one button can be used for wake up source
      esp_sleep_enable_ext1_wakeup( bit_mask, ESP_EXT1_WAKEUP_ALL_LOW );
    #endif
    esp_deep_sleep_start();
  } 
  else { // if we don't have buttons on RTC GPIOs 
    u8g2.setFont(MEDIUM_FONT);
    textAnimation("Button 1 wakes",3000);
    textAnimation("Sleep",1000);
    splash_off(); 

    DEBUG("Light sleep");
    #ifdef ACTIVE_HIGH
      gpio_wakeup_enable(static_cast <gpio_num_t> (joystick_button_gpio[0]), GPIO_INTR_HIGH_LEVEL );
    #else
      gpio_wakeup_enable(static_cast <gpio_num_t> (joystick_button_gpio[0]), GPIO_INTR_LOW_LEVEL );
    #endif
    esp_sleep_enable_gpio_wakeup();
    esp_light_sleep_start();
    esp_restart();
  }
};

// Respawn
void ESP_on () {
  uint8_t GPIO;  
  esp_sleep_wakeup_cause_t wakeup_cause;
  wakeup_cause = esp_sleep_get_wakeup_cause();
  switch(wakeup_cause)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : DEBUG("Wakeup caused by ext0 signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : 
      DEBUG("Wakeup caused by ext1 signal using RTC_CNTL");
      GPIO = log(esp_sleep_get_ext1_wakeup_status() )/log(2); 
      DEBUG("Woken up by GPIO_" + (String)(GPIO));
      break;
    case ESP_SLEEP_WAKEUP_TIMER : DEBUG("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : DEBUG("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : DEBUG("Wakeup caused by ULP program"); break;
    default : DEBUG("Wakeup was not caused by deep sleep: " + (String)(wakeup_cause)); break;
  }
  splash_on();
}

void splash_on() {
  u8g2.setFont(MEDIUM_FONT);
  textAnimation(".",200,-5);
  textAnimation("*",100, 3);
  textAnimation("x",100,-3);
  textAnimation("X",100,-1);
  textAnimation("x",100,-3);
  textAnimation(".",100,-5);
  textAnimation("-",50,-3);
  textAnimation("___",50,-8);
  textAnimation("_____",50,-8);
  textAnimation("_______",50,-8);
  textAnimation("_________",40,-8);
  textAnimation("___________",30,-8);
  textAnimation("_____________",20,-8);
  textAnimation("_______________",10,-8);
}


void splash_off() {
  u8g2.setFont(MEDIUM_FONT);
  String s = "_________________";
  for (int i=0; i<8; i++) {
    s = s.substring(i);
    textAnimation(s,70,-8);
  }
  textAnimation(".",200,-5);
  textAnimation("*",100,3);
  textAnimation("x",100,-3);
  textAnimation("X",100,-1);
  textAnimation("x",100,-3);
  textAnimation(".",500,-5);
  textAnimation("",10,-5);
}
