// Protector
#ifndef __FUNCTIONS__
#define __FUNCTIONS__

//Includes
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>

#include "vardelay.h"
#include "analog.h"
#include "serial.h"

//Defines
#define CMD_OK 0x00
#define CMD_NOT_KNOWN 0x01
#define CMD_DEST_UNREACHABLE 0x02
#define CMD_LOCK_MIN_X1 0x03
#define CMD_LOCK_MAX_X1 0x04
#define CMD_LOCK_MIN_X2 0x05
#define CMD_LOCK_MAX_X2 0x06
#define CMD_LOCK_MIN_Y 0x07
#define CMD_LOCK_MAX_Y 0x08
#define CMD_LOCK_Z 0x09

#define MOT_NOT_KNOWN 0x01
#define MOT_NOT_KNOWN_GET 0xFFFF

#define WAIT_FOR_NEXT_FRAME 0x0A
#define WAIT_FOR_IT_1 0x0B
#define WAIT_FOR_IT_2 0x0C


#define MAX_DEST_X 23505
#define MAX_DEST_Y 18021
#define MAX_DEST_Z 6400

#define PIN_SENSOR_X1_MIN (PINH & 0x08)
#define PIN_SENSOR_X1_MAX (PINH & 0x10)
#define PIN_SENSOR_X2_MIN (PINH & 0x20)
#define PIN_SENSOR_X2_MAX (PINH & 0x40)
#define PIN_SENSOR_Y_MIN (PING & 0x01)
#define PIN_SENSOR_Y_MAX (PING & 0x02)

// Functions
void init_port(void);
uint8_t setDest (uint8_t selectedMotor, uint16_t destination);
uint8_t setMS (uint8_t selectedMotor, uint8_t microStep);
uint8_t setPump (bool onOff);
uint8_t setSpeed (uint8_t selectedMotor, uint8_t delayStep);
bool isContactTouched (void);
uint8_t whichContactTouched(void);
uint8_t setADC(uint16_t adcLevel);
bool isPotsUnderLimit(void);
uint8_t setMotLocked(uint8_t selectedMotor, bool lockedOnOff);

uint16_t getDest (uint8_t selectedMotor);
uint16_t getMS (void);
uint8_t getPump(void);
uint16_t getADC(void);
uint8_t getRelease(void);
uint16_t getADCvalue(uint8_t selectedPin);
uint8_t getSpeed (uint8_t selectedMotor);
uint8_t getMotLocked(void);

#endif
