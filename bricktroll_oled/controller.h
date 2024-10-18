#pragma once

enum              eJoystic                    {JOY_LEFT, JOY_RIGHT};
enum              eRemoteMode                 {RM_L_ALL, RM_R_ALL, RM_L_THROTTLE_R_STEER, RM_L_STEER_R_THROTTLE, RM_L_ALL_90CW, RM_R_ALL_90CCW, RM_NUM_MODES};
static const int  joystick_axe_gpio[4]      = {JOY_LEFT_X_GPIO, JOY_LEFT_Y_GPIO, JOY_RIGHT_X_GPIO, JOY_RIGHT_Y_GPIO};
static const int  joystick_button_gpio[2]   = {JOY_LEFT_SW_GPIO, JOY_RIGHT_SW_GPIO};
static int        zero_offset[4]            = {2048,2048,2048,2048};

struct controlMode {
  eRemoteMode   control_mode;
  bool          left_required;
  bool          right_required;
  char          display_name[20];  
} ;

static const controlMode modes[RM_NUM_MODES] = {
  // mode                   left      right     name
  {RM_L_ALL ,               true ,    false,    "Left joystick only"},
  {RM_R_ALL ,               false,    true,     "Right joystick only"},
  {RM_L_THROTTLE_R_STEER,   true,     true,     "L = thr, R = steer"},
  {RM_L_STEER_R_THROTTLE,   true,     true,     "L = steer, R = thr"},
  {RM_L_ALL_90CW,           true,     false,    "Left joyst. 90 CW"},
  {RM_R_ALL_90CCW,          false,    true,     "Right joyst. 90 CCW"}
};

struct Position {
    int16_t x;
    int16_t y;
    bool operator==(Position& o);
    bool operator!=(Position& o);
};

struct Keys {
    boolean a;
    boolean b;
    bool pressed();
};


void controller_init();

Position read_joystick(int joystick_num);

Keys read_keys();

int normalize(int in, int zero_pos);

Position remap( Position posL, Position posR, eRemoteMode rMode);
