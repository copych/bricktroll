//#include <esp_wifi.h>
//#include <esp_task_wdt.h>
#include "config.h"
#include <U8g2lib.h>
#include "Lpf2Hub.h"
#include "Lpf2HubConst.h"
#include "controller.h"
#include "rcmodel.h"
#include "ui.h"

#if defined DISPLAY_SSD1306
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8_ROTATE, U8X8_PIN_NONE, DISPLAY_SCL, DISPLAY_SDA) ;
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8_ROTATE, U8X8_PIN_NONE, DISPLAY_SCL, DISPLAY_SDA) ; 
#elif defined DISPLAY_SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8_ROTATE, U8X8_PIN_NONE, DISPLAY_SCL, DISPLAY_SDA) ;
#elif defined DISPLAY_SSD1305
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8_ROTATE, U8X8_PIN_NONE, DISPLAY_SCL, DISPLAY_SDA) ;
#endif

RcModel* model;
Lpf2Hub legoHub;
static int num_joysticks = 2;
static int RTC_present = 0;
static int RTC_1st = -1;
static int sw_RTC[2];
static eRemoteMode control_mode = RM_L_ALL;
bool joy_present[2] = {true,true};

void menu() {
    bool done = false;
    textAnimation("Monster <-> Rally");
    
    Keys keys = {false, false};
    Position pos;
    while (1) {
      keys = read_keys();  
        if(keys.a){
            model = new MonsterTruck(legoHub);
            textAnimation("MonsterTrack");  
            done = true;          
            break;
        } else if (keys.b) {
            model = new RallyCar(legoHub);
            textAnimation("RallyCar");
            done = true;
            break;
        }
      for (int i = 0 ; i<2 ; i++) {
        pos = read_joystick(i);
        if (pos.x == -100) {
            model = new MonsterTruck(legoHub);
            textAnimation("MonsterTrack");
            done = true;
            break;
        } else if (pos.x == 100) {
            model = new RallyCar(legoHub);
            textAnimation("RallyCar");
            done = true;
            break;
        }
        if (done) break;
      }
      if (done) break;
    }
}

void setup()
{
    Serial.begin(115200);

// they say wifi is off by default
//   esp_wifi_deinit();
   
    u8g2.begin();
    
    u8g2.clearBuffer();
    u8g2.setFont(SMALL_FONT); 
    u8g2.sendBuffer();
     
    u8g2.setFont(MEDIUM_FONT);
   
    splash_on();
    
    controller_init();


    menu();
}

void auto_shutdown()
{
    static int counter = 0;
    if (legoHub.isConnected()) {
        counter = 0;
    } else {
        if (counter > 100) {
            textAnimation("Shutting down");
            ESP_off();
        }
        counter++;
    }
}

void loop()
{
    Position posL, posR, pos;
    model->connect();
    model->calibrate();
    posL = read_joystick(0);
    if (num_joysticks > 1 ) posR = read_joystick(1);
    pos = remap(posL, posR, control_mode);
    
    model->control(pos);
    auto_shutdown();

}
