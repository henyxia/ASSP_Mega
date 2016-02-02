// Protector
#ifndef __FUNCTIONS__
#define __FUNCTIONS__

//Includes
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>

#include "vardelay.h"

//Defines


// Functions
void init_port(void);
uint8_t setDest (uint8_t selectedMotor, uint16_t destination);
uint8_t setMS (uint8_t selectedMotor, uint8_t microStep);
uint8_t setPump (bool onOff);
uint8_t setSpeed (uint8_t selectedMotor, uint8_t delayStep);


#endif