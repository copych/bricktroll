#include "rcmodel.h"

byte portA = (byte)ControlPlusHubPort::A;
byte portB = (byte)ControlPlusHubPort::B;
byte portC = (byte)ControlPlusHubPort::C;
byte portD = (byte)ControlPlusHubPort::D;

static int steering_pos;

void setSteeringPos(int sp) {
    steering_pos = sp;
}

int getSteeringPos() {
    return steering_pos;
}

void tachoMotorCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
    Lpf2Hub *myHub = (Lpf2Hub *)hub;

    DEB("cb port: ");
    DEBUG(portNumber, DEC);
    int rotation = myHub->parseTachoMotor(pData);
    DEB("Rotation: ");
    DEB(rotation, DEC);
    setSteeringPos(rotation); 
}

void RcModel::initalizeCallback(byte port) {
    if (!isInitialized) {
        DEBUG("Callback initialized");
        hub.activatePortDevice(port, tachoMotorCallback);
        delay(200);
        isInitialized = true;
    }
}

void RcModel::connect() {
    if (!hub.isConnected() && !hub.isConnecting()) {
        hub.init(); 
    }
    if (hub.isConnecting()) {
        hub.connectHub();
        if (hub.isConnected()) {
            textAnimation("Connected to HUB");
        } else {
            textAnimation("Failed to connected to HUB");
        }
    }
    if (!hub.isConnected()) {
        textAnimation("wait.", 200);
        textAnimation("wait..", 200);
        textAnimation("wait", 200);
        isInitialized = false;

    }
}

void RcModel::calibrate(byte port) {
    if (hub.isConnected() && !isCalibrated) {
        initalizeCallback(port);

        textAnimation("Calibrating :");  
        
        hub.setAbsoluteMotorEncoderPosition(port, 0); 


        hub.setTachoMotorSpeedForDegrees(port, -40, 180);
        textAnimation("<< all left"); 
        int max_pos = getSteeringPos();
              
        hub.setTachoMotorSpeedForDegrees(port, 40, 180);
        textAnimation("all right >>");
        int min_pos = getSteeringPos();

        DEB("Min: " + String(min_pos) + " Max: " + String(max_pos));
        int center = (min_pos + max_pos)/2;
        hub.setAbsoluteMotorPosition(port, 10, center-10);
        delay(500);
        hub.setAbsoluteMotorPosition(port, 10, center+5);
        delay(500);
        hub.setAbsoluteMotorPosition(port, 10, center);
        delay(500);
        textAnimation("> center <");
      
        hub.setAbsoluteMotorEncoderPosition(port, 0);
        textAnimation(".", 500);
        textAnimation(" Calibration complete");

        isCalibrated = true;
    }

}

void RallyCar::calibrate() 
{
    RcModel::calibrate(portB);
}

void RallyCar::control(Position pos) {
    static Position prev_pos;

    if (hub.isConnected()) {
        if (prev_pos != pos) {
            hub.setBasicMotorSpeed(portD, pos.x);
            hub.setAbsoluteMotorPosition(portB, 50, -pos.y);
            textAnimation(String(pos.x) + " " + String(pos.y),5);
        }

        prev_pos.x = pos.x;
        prev_pos.y = pos.y;
    }
}

void MonsterTruck::calibrate() 
{
    RcModel::calibrate(portC);
}

void MonsterTruck::control(Position pos) {
    static Position prev_pos;

    if (hub.isConnected()) {
        if (prev_pos != pos) {
            hub.setBasicMotorSpeed(portA, pos.y);
            hub.setBasicMotorSpeed(portB, pos.y);
            hub.setAbsoluteMotorPosition(portC, 100, pos.x);
            textAnimation(String(pos.x) + " " + String(pos.y),0);

        }

        prev_pos.x = pos.x;
        prev_pos.y = pos.y;
    }
}
