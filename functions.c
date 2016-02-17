/* Pinout Arduino :

  // OUTPUTS
 * Pump : Port G (PG2) (Pin 39)

 * Motor X1 : Port A
 * Motor X2 : Port L
 * Motor Y : Port B
 * Motor Z : Port C
 * Motor RotZ : Port F

 * Bit Port :   7   |   6   |   5    |  4   ||  3   |   2   |   1   |   0
 * Bit Driv :        |        | Dir | ~Enab ||  MS3  | MS2 |  MS1 | Step

  // INPUTS
 * Position Sensor X1 Min : Port H (PH3) (Pin 6)
 * Position Sensor X1 Max : Port H (PH4) (Pin 7)
 * Position Sensor X2 Min : Port H (PH5) (Pin 8)
 * Position Sensor X2 Max : Port H (PH6) (Pin 9)

 * Position Sensor Y Min : Port G (PG0) (Pin 41)
 * Position Sensor Y Max : Port G (PG1) (Pin 40)

 * Linear potentiometer 1 (Analog In) : Port F (PF6) (Analog pin 6)
 * Linear potentiometer 2 (Analog In) : Port F (PF7) (Analog pin 7)
 * Linear potentiometer 3 (Analog In) : Port K (PK0) (Analog pin 8)

*/

// Includes
#include "functions.h"

#define DEFAULT_DELAY 10

//Globals
uint16_t oldDestX = 0, oldDestY = 0, oldDestZ = 0, oldDestRotZ = 0;
uint8_t MSX = 0, MSY = 0, MSZ = 0, MSRotZ = 0;
uint8_t delayStepX = DEFAULT_DELAY; //MAX is 255 ms
uint8_t delayStepY = DEFAULT_DELAY;
uint8_t delayStepZ = DEFAULT_DELAY;
uint8_t delayStepRotZ = DEFAULT_DELAY;

bool onOffPump = 0;
bool onOffMotX = 0; //Motors not locked by default
bool onOffMotY = 0;
bool onOffMotZ = 0;
bool onOffMotRotZ = 0;

uint16_t maxDestX = 23505;
uint16_t maxDestY = 18021;
uint16_t maxDestZ = 6400;

uint16_t potLimit = 512; //Mid position by default

uint16_t i =0, stepToDo = 0;

// Functions

    // Init motor, sensor and pump ports
void init_port(void)
{

    // Init pinmode (as outputs (1) or inputs (0))
      //Motors ports as outputs
    DDRA |= 0b00111111;
    DDRL |= 0b00111111;
    DDRB |= 0b00111111;
    DDRC |= 0b00111111;
    DDRF |= 0b00111111;

      // Pump (PG2) as output, sensors Y (PG0 and PG1) as inputs
    DDRG &= ~0b00000011;
    DDRG |= 0b00000100;

      //Sensors X1 and X2 as inputs
    DDRH &= ~0b01111000;

      //Pots as inputs
    DDRK &= ~0b00000001;
    DDRF &= ~0b11000000;

    // Init values (Drivers disabled by default at init, and reversed logic on Enable pin)
      //Pin enable HIGH
    PORTA |= 0b00010000;
    PORTL |= 0b00010000;
    PORTB |= 0b00010000;
    PORTC |= 0b00010000;
    PORTF |= 0b00010000;
      //Pin MS3,2,1 LOW (Full step by default)
    PORTA &= ~0b00001110;
    PORTL &= ~0b00001110;
    PORTB &= ~0b00001110;
    PORTC &= ~0b00001110;
    PORTF &= ~0b00001110;

    PORTG &= ~0b00000100; //Pump disabled at init

    //Activate Pull up internal res on Arduino for contact sensors
    PORTH |= 0b01111000;
    PORTG |= 0b00000011;
}

    //Tell if a contact sensor is touched
bool isContactTouched (void)
{
    return ((PIN_SENSOR_X1_MIN == 0) || (PIN_SENSOR_X1_MAX == 0) || (PIN_SENSOR_X2_MIN == 0) || (PIN_SENSOR_X2_MAX == 0) || (PIN_SENSOR_Y_MIN == 0) || (PIN_SENSOR_Y_MAX == 0));
}

    //Tell which contact sensor is touched and its associated error code
uint8_t whichContactTouched (void)
{
    if (PIN_SENSOR_X1_MIN == 0)
        return CMD_LOCK_MIN_X1;
    else if (PIN_SENSOR_X1_MAX == 0)
        return CMD_LOCK_MAX_X1;
    else if (PIN_SENSOR_Y_MIN == 0)
        return CMD_LOCK_MIN_Y;
    else if (PIN_SENSOR_Y_MAX == 0)
        return CMD_LOCK_MAX_Y;
    else if (PIN_SENSOR_X2_MIN == 0)
        return CMD_LOCK_MIN_X2;
    else if (PIN_SENSOR_X2_MAX == 0)
        return CMD_LOCK_MAX_X2;
    else
        return CMD_OK;
}

    //Execute the step order, compared to the reference position (initial position)
uint8_t setDest (uint8_t selectedMotor, uint16_t destination)
{
    switch(selectedMotor)
    {
        // X
    case 0 :
    {
            //Checking if destination is reachable
            if (destination > maxDestX)
                return CMD_DEST_UNREACHABLE;

            //Set Dir
            if (destination >= oldDestX) // Go ascending X, DirX = 1 (OK !)
            {
                PORTA |= 0x20;
                PORTL |= 0x20;

                stepToDo = destination - oldDestX;
            }

            else
            {
                PORTA &= ~0x20;
                PORTL &= ~0x20;

                stepToDo = oldDestX - destination;
            }

            // Remember where we are
            oldDestX = destination;

            //Enable motors
            PORTA &= ~0x10; //Reversed logic on Enable pin
            PORTL &= ~0x10;

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if (!isContactTouched())
                {
                    PORTA |= 0x01; //High
                    PORTL |= 0x01;
                    delay_ms(delayStepX);
                    PORTA &= ~0x01; //Low
                    PORTL &= ~0x01;
                    delay_ms(delayStepX);
                }

                else
                    return whichContactTouched();
            }

            //Disable motors after movement if wanted
            if (!onOffMotX)
            {
                PORTA |= 0x10;
                PORTL |= 0x10;
            }

            return CMD_OK;
    }
        break;

        // Y
        case 1 :
    {
            //Checking if destination is reachable
            if (destination > maxDestY)
                return CMD_DEST_UNREACHABLE;

            //Set Dir
            if (destination >= oldDestY) // Go ascending Y, DirY = 1 (OK !)
            {
                PORTB |= 0x20;

                stepToDo = destination - oldDestY;
            }

            else
            {
                PORTB &= ~0x20;

                stepToDo = oldDestY - destination;
            }

            // Remembering where we are
            oldDestY = destination;

            //Enable motor
            PORTB &= ~0x10; //Reversed logic on Enable pin

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if (!isContactTouched())
                {
                    PORTB |= 0x01; //High
                    delay_ms(delayStepY);
                    PORTB &= ~0x01; //Low
                    delay_ms(delayStepY);
                }

                else
                    return whichContactTouched();
            }

            //Disable motor after movement if wanted
            if (!onOffMotY)
                PORTB |= 0x10;

            return CMD_OK;
    }
        break;

        // Z
        case 2 :
    {
            //Checking if destination is reachable
            if (destination > maxDestZ)
                return CMD_DEST_UNREACHABLE;

            //Set Dir
            if (destination >= oldDestZ) // Go ascending Z, DirZ = 0 (OK)
            {
                PORTC |= 0x20;

                stepToDo = destination - oldDestZ;
            }

            else
            {
                PORTC &= ~0x20;

                stepToDo = oldDestZ - destination;
            }

            // Remembering where we are
            oldDestZ = destination;

            //Enable motor
            PORTC &= ~0x10; //Reversed logic on Enable pin

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if ((!isContactTouched()) && (isPotsUnderLimit()))
                {
                    PORTC |= 0x01; //High
                    delay_ms(delayStepZ);
                    PORTC &= ~0x01; //Low
                    delay_ms(delayStepZ);
                }

                else if (!isPotsUnderLimit())
                    return CMD_LOCK_Z;

                else
                    return whichContactTouched();
            }

            //Disable motor after movement if wanted
            if (!onOffMotZ)
                PORTC |= 0x10;

            return CMD_OK;
    }
        break;

        //Rot Z
        case 3 :
    {
            //Set Dir
            if (destination >= oldDestRotZ) // Go direct sense RotZ, DirRotZ = 0 (else 1 for Trigo)
            {
                PORTF |= 0x20;

                stepToDo = destination - oldDestRotZ;
            }

            else
            {
                PORTF &= ~0x20;

                stepToDo = oldDestRotZ - destination;
            }

            // Remembering where we are
            oldDestRotZ = destination;

            //Enable motor
            PORTF &= ~0x10; //Reversed logic on Enable pin

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if ((!isContactTouched()) && (isPotsUnderLimit()))
                {
                    PORTF |= 0x01; //High
                    delay_ms(delayStepRotZ);
                    PORTF &= ~0x01; //Low
                    delay_ms(delayStepRotZ);
                }

                else if (!isPotsUnderLimit())
                    return CMD_LOCK_Z;

                else
                    return whichContactTouched();
            }

            //Disable motor after movement if wanted
            if (!onOffMotRotZ)
                PORTF |= 0x10;

            return CMD_OK;
    }
        break;

        //Error : unknown selected motor
        default :
            return MOT_NOT_KNOWN;
    }
}

    //Set the delay between two steps (in ms) (MAX = 255 ms)
uint8_t setSpeed(uint8_t selectedMotor, uint8_t delayStep)
{
    if (delayStep != 0)
    {
        switch(selectedMotor)
        {
            // X
        case 0 :
            delayStepX = delayStep;

            break;

            // Y
        case 1 :
            delayStepY = delayStep;

            break;

            // Z
        case 2 :
            delayStepZ = delayStep;

            break;

            //Rot Z
        case 3 :
            delayStepRotZ = delayStep;

            break;

            //Error : unknown selected motors
        default :
            return MOT_NOT_KNOWN;
        }

    return CMD_OK;
    }

    //Delay cannot be 0 ms !!
    else
        return CMD_DEST_UNREACHABLE;
}

    //Set the micro-step factor of the selected motor (0 : Full Step, 1 : Half, ... , 4 : Sixteenth)
uint8_t setMS (uint8_t selectedMotor, uint8_t microStep)
{
    switch(selectedMotor)
    {
        // X
        case 0 :

            switch(microStep)
            {
                case 0 :
            {
                    PORTA &= ~0b00001110;
                    PORTL &= ~0b00001110;
                    MSX = 0;
            }
                break;

                case 1 :
            {
                    PORTA |= 0b00000010;
                    PORTL |= 0b00000010;
                    PORTA &= ~0b00001100;
                    PORTL &= ~0b00001100;
                    MSX = 1;
            }
                break;

                case 2 :
            {
                    PORTA |= 0b00000100;
                    PORTL |= 0b00000100;
                    PORTA &= ~0b00001010;
                    PORTL &= ~0b00001010;
                    MSX = 2;
            }
                break;

                case 3 :
            {
                    PORTA |= 0b00001100;
                    PORTL |= 0b00001100;
                    PORTA &= ~0b00000010;
                    PORTL &= ~0b00000010;
                    MSX = 3;
            }
                break;

                case 4 :
            {
                    PORTA |= 0b00001110;
                    PORTL |= 0b00001110;
                    MSX = 4;
            }
                break;

                default :
                    return CMD_DEST_UNREACHABLE; // Error : bad microStep factor
            }

        break;

        // Y
        case 1 :

            switch(microStep)
            {
                case 0 :
            {
                    PORTB &= ~0b00001110;
                    MSY = 0;
            }
                break;

                case 1 :
            {
                    PORTB |= 0b00000010;
                    PORTB &= ~0b00001100;
                    MSY = 1;
            }
                break;

                case 2 :
            {
                    PORTB |= 0b00000100;
                    PORTB &= ~0b00001010;
                    MSY = 2;
            }
                break;

                case 3 :
            {
                    PORTB |= 0b00001100;
                    PORTB &= ~0b00000010;
                    MSY = 3;
            }
                break;

                case 4 :
            {
                    PORTB |= 0b00001110;
                    MSY = 4;
            }
                break;

                default :
                    return CMD_DEST_UNREACHABLE; // Error : bad microStep factor
            }

        break;

        // Z
        case 2 :

            switch(microStep)
            {
                case 0 :
            {
                    PORTC &= ~0b00001110;
                    MSZ = 0;
            }
                break;

                case 1 :
            {
                    PORTC |= 0b00000010;
                    PORTC &= ~0b00001100;
                    MSZ = 1;
            }
                break;

                case 2 :
            {
                    PORTC |= 0b00000100;
                    PORTC &= ~0b00001010;
                    MSZ = 2;
            }
                break;

                case 3 :
            {
                    PORTC |= 0b00001100;
                    PORTC &= ~0b00000010;
                    MSZ = 3;
            }
                break;

                case 4 :
            {
                    PORTC |= 0b00001110;
                    MSZ = 4;
            }
                break;

                default :
                    return CMD_DEST_UNREACHABLE; // Error : bad microStep factor
            }

            break;

        // RotZ
        case 3 :

            switch(microStep)
            {
                case 0 :
            {
                    PORTF &= 0b00001110;
                    MSRotZ = 0;
            }
                break;

                case 1 :
            {
                    PORTF |= 0b00000010;
                    PORTF &= ~0b00001100;
                    MSRotZ = 1;
            }
                break;

                case 2 :
            {
                    PORTF |= 0b00000100;
                    PORTF &= ~0b00001010;
                    MSRotZ = 2;
            }
                break;

                case 3 :
            {
                    PORTF |= 0b00001100;
                    PORTF &= ~0b00000010;
                    MSRotZ = 3;
            }
                break;

                case 4 :
            {
                    PORTF |= 0b00001110;
                    MSRotZ = 4;
            }
                break;

                default :
                    return CMD_DEST_UNREACHABLE; // Error : bad microStep factor
            }

            break;

            //Error : unknown selected motor
            default :
                return CMD_NOT_KNOWN;
    }

    return CMD_OK;
}

    //Command the transistor commanding the pneumatic distributor
uint8_t setPump (bool onOff)
{
    onOffPump = onOff;

    if (onOff)
        PORTG |= 0x04;
    else
        PORTG &= ~0x04;

    return CMD_OK;
}

    //Set limit of accepted level on linear pots<=>applied force on needle forZ
uint8_t setADC(uint16_t adcLevel)
{
    potLimit = adcLevel;
    return CMD_OK;
}

    //Tell if the value read on linear pots is under the limit
bool isPotsUnderLimit(void)
{
    uint16_t adcValue1, adcValue2;

    //Reading ADC
    adcValue1 = analogRead(6);
    adcValue2 = analogRead(7);

    return ((adcValue1 < potLimit) && (adcValue2 < potLimit));
}

    //Give the current position of the selected motor
uint16_t getDest(uint8_t selectedMotor)
{
    switch(selectedMotor)
    {

    //X position wanted
    case 0 :
    {
        return oldDestX;
    }
        break;

        //Y position wanted
    case 1 :
    {
        return oldDestY;
    }
        break;

        //Z position wanted
    case 2 :
    {
        return oldDestZ;
    }
        break;

        //RotZ position wanted
    case 3 :
    {
        return oldDestRotZ;
    }
        break;

    default :
        return MOT_NOT_KNOWN_GET;
    }
}

    //Give the converted value on the selected analogic pin
uint16_t getADCvalue(uint8_t selectedPin)
{
    switch(selectedPin)
    {

    // PK0 wanted
    case 0 :
    {
        return (analogRead(6) & 0x03FF);
    }
        break;

        // PK1 wanted
    case 1 :
    {
        return (analogRead(7) & 0x03FF);
    }
        break;

        // PK2 wanted
    case 2 :
    {
        return (analogRead(8) & 0x03FF);
    }
        break;

    default :
        return 0;
    }
}

    //Give the current values of MS settings of all motor and build the frames
uint16_t getMS(void)
{
    return ((MSY & 0x01) << 11 | MSX << 8 | MSRotZ << 5 | MSZ << 2 | MSY >> 1);
}

    //Give pump state (on = 1, off = 0)
uint8_t getPump(void)
{
    return onOffPump;
}

    //Give the set limit for pots on Z axis
uint16_t getADC(void)
{
    return potLimit;
}

    //Give the set speed of the selected motor
uint8_t getSpeed (uint8_t selectedMotor)
{
    switch(selectedMotor)
    {

    //X position wanted
    case 0 :
    {
        return delayStepX;
    }
        break;

        //Y position wanted
    case 1 :
    {
        return delayStepY;
    }
        break;

        //Z position wanted
    case 2 :
    {
        return delayStepZ;
    }
        break;

        //RotZ position wanted
    case 3 :
    {
        return delayStepRotZ;
    }
        break;

        //Error : motor not known
    default :
        return 0;
    }
}

    //Enable or disable the selected motor (motors not locked by default)
uint8_t setMotLocked(uint8_t selectedMotor, bool lockedOnOff)
{
    switch(selectedMotor)
    {
    //X
    case 0 :
        if (!lockedOnOff)
        {
            PORTA |= 0x10;
            PORTL |= 0x10;
        }
        else
        {
            PORTA &= ~0x10; //Reversed logic on Enable pin
            PORTL &= ~0x10;
        }
        break;

        //Y
    case 1 :
        if (!lockedOnOff)
            PORTB |= 0x10;
    else
            PORTB &= ~0x10;
        break;

        //Z
    case 2 :
        if (!lockedOnOff)
            PORTC |= 0x10;
        else
            PORTC &= ~0x10;
        break;

        //RotZ
    case 3 :
        if (!lockedOnOff)
            PORTF |= 0x10;
        else
            PORTF &= ~0x10;
        break;

        //Error : motor not known
    default :
        return MOT_NOT_KNOWN;
    }

    return CMD_OK;
}

    //Tell the state (enable or disable) of every motors and build the associated frame
uint8_t getMotLocked(void)
{
    return ((onOffMotX | (onOffMotY << 1 ) | (onOffMotZ << 2) | (onOffMotRotZ << 3)));
}
