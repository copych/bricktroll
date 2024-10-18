#include "config.h"
#include "controller.h"


void controller_init() {
    for (int i=0; i<4 ; i++) pinMode(joystick_axe_gpio[i], INPUT); // axes GPIO setup
    for (int i=0; i<2 ; i++) pinMode(joystick_button_gpio[i], INPUT_PULLUP); // switches GPIO setup
    delay(20);
    for (int i=0; i<4 ; i++) zero_offset[i] = analogRead(joystick_axe_gpio[i]); // zero_offset
    if (zero_offset[0] < 500 || zero_offset[1] < 100) {
      textAnimation("No Left Joystick!", 1000);
      joy_present[0] = false;
      ESP_off();
    }
    if (zero_offset[2] < 500 || zero_offset[3] < 100) {
      textAnimation("No Right Joystick!", 1000);
      joy_present[1] = false;
    }
}

int normalize(int in, int zero_pos) {
    int out;
    if (in > zero_pos) {
      out = map(in, zero_pos, 4095, 0, 100 );
    } else {
      out = map(in, 0, zero_pos, -100, 0 );
    }
    if (abs(out)<3) out = 0;
    return out;
}



Position read_joystick(int joystick_num) {
    Position pos;
    if (joy_present[joystick_num]) {
      pos.x = normalize(analogRead(joystick_axe_gpio[2 * joystick_num]) , zero_offset[2 * joystick_num]);
      pos.y = normalize(analogRead(joystick_axe_gpio[2 * joystick_num + 1]) , zero_offset[2 *joystick_num + 1]);
    } else {
      pos.x = 0;
      pos.y = 0;
    }
    return pos;
}

bool Position::operator==(Position& o) {
   return x == o.x && y == o.y;
}

bool Position::operator!=(Position& o) {
   return !(*this == o);
}

Position remap(Position posL, Position posR, eRemoteMode rMode) {
  Position out ;
  switch (rMode) {
    case RM_R_ALL:
      out = posR;
      break;
    case RM_L_THROTTLE_R_STEER:
      out.y = posL.y;
      out.x = posR.x;
      break;
    case RM_L_STEER_R_THROTTLE:
      out.x = posL.x;
      out.y = posR.y;
      break;
    case RM_L_ALL_90CW:
      out.x = posL.y;
      out.y = -posL.x;
      break;
    case RM_R_ALL_90CCW:
      out.x = -posR.y;
      out.y = posR.x;
      break;
    case RM_L_ALL:
    default:
      out = posL;
  }
  return out;
}

Keys read_keys(){
    Keys keys;
    keys.a = digitalRead(joystick_button_gpio[0]) == 0;
    keys.b = digitalRead(joystick_button_gpio[1]) == 0;
    return keys;
}

boolean Keys::pressed(){
    return this->a || this->b;
}
