#pragma once

// a simple script to show a text string during a given time
void textAnimation(const String& str, uint32_t msDelay=1000, int32_t yShift=0, bool show=true)  ;


int Check_RTC()  ;


// Stand-by mode with some fun
void ESP_off();

// Respawn
void ESP_on ();

void splash_on() ;
void splash_off() ;
