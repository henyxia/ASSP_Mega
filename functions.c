/* Pinout Arduino :

 * Motor X1 : Port A
 * Motor X2 : Port L
 * Motor Y : Port B
 * Motor Z : Port C
 * Motor RotZ : Port F

 * Bit Port :   7   |   6   |   5    |     4     ||     3    |    2    |    1    |   0
 * Bit Driv :       |        |   Dir | ~Enab ||  MS3  | MS2 |  MS1 | Step


 * Position Sensor X1 Min : Port D (PD0) (Pin 21)
 * Position Sensor X1 Max : Port D (PD1) (Pin 20)
 * Position Sensor X2 Min : Port D (PD2) (Pin 19)
 * Position Sensor X2 Max : Port D (PD3) (Pin 18)

 * Position Sensor Y Min : Port G (PG0) (Pin 41)
 * Position Sensor Y Max : Port G (PG1) (Pin 40)

 * Pump : Port H (H0) (Pin 17)

*/

// Includes
#include "functions.h"

//Globals
uint16_t oldDestX, oldDestY, oldDestZ, oldDestRotZ = 0;
uint8_t MSX, MSY, MSZ, MSRotZ = 1;
uint8_t delayStepX, delayStepY, delayStepZ, delayStepRotZ = 30;

// Functions

    // Init motor, sensor and pump ports
void init_port(void)
{
    // Init pinmode (as outputs (1) or inputs (0))
    DDRA |= 0b00111111;
    DDRL |= 0b00111111;
    DDRB |= 0b00111111;
    DDRC |= 0b00111111;
    DDRF |= 0b00111111;

    DDRD &= 0x0A;
    DDRG &= 0x03;

    DDRH |= 0x01;

    // Init values (Drivers disabled by default at init, and reversed logic on Enable pin)
    PORTA |= 0b00010000;
    PORTL |= 0b00010000;
    PORTB |= 0b00010000;
    PORTC |= 0b00010000;
    PORTF |= 0b00010000;

    PORTH &= 0x01;

    //Activate Pull up internal res on Arduino for contact sensors
    PORTD |= 0b00001111;
    PORTG |= 0b00000011;
}

    //Tell if a contact sensor is touched
bool isContactTouched (void)
{
    return ((PD0 == 0) || (PD1 == 0) || (PD2 == 0) || (PD3 == 0) || (PG0 == 0) || (PG1 == 0));
}

    //Tell which contact sensor is touched and its associated error code
uint8_t whichContactTouched (void)
{
    if (PD0 == 0)
        return 3;
    else if (PD1 == 0)
        return 4;
    else if (PD2 == 0)
        return 5;
    else if (PD3 == 0)
        return 6;
    else if (PG0 == 0)
        return 7;
    else if (PG1 == 0)
        return 8;
    else
        return 0;
}

    //Execute the step order, compared to the reference position (initial position)
uint8_t setDest (uint8_t selectedMotor, uint16_t destination)
{
    uint16_t i, stepToDo = 0;

    switch(selectedMotor)
    {
        // X
        case 0 :

            //Set Dir
            if (destination >= oldDestX) // Go ascending X, DirX = 1 (TO VERIFY !!)
            {
                PORTA |= 0x20;
                PORTL |= 0x20;

                stepToDo = destination - oldDestX;
            }

            else
            {
                PORTA &= 0x20;
                PORTL &= 0x20;

                stepToDo = oldDestX - destination;
            }

            // Remember where we are
            oldDestX = destination;

            //Enable motors
            PORTA &= 0x10; //Reversed logic on Enable pin
            PORTL &= 0x10;

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if (!isContactTouched())
                {
                    PORTA |= 0x01; //High
                    PORTL |= 0x01;
                    delay_ms(delayStepX);
                    PORTA &= 0x01; //Low
                    PORTL &= 0x01;
                    delay_ms(delayStepX);
                }

                else
                    return whichContactTouched();
            }

            //Disable motors
            PORTA |= 0x10;
            PORTL |= 0x10;

            return 0;

        break;

        // Y
        case 1 :

            //Set Dir
            if (destination >= oldDestY) // Go ascending Y, DirY = 1 (TO VERIFY !!)
            {
                PORTB |= 0x20;

                stepToDo = destination - oldDestY;
            }

            else
            {
                PORTB &= 0x20;

                stepToDo = oldDestY - destination;
            }

            // Remembering where we are
            oldDestY = destination;

            //Enable motor
            PORTB &= 0x10; //Reversed logic on Enable pin

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if (!isContactTouched())
                {
                    PORTB |= 0x01; //High
                    delay_ms(delayStepY);
                    PORTB &= 0x01; //Low
                    delay_ms(delayStepY);
                }

                else
                    return whichContactTouched();
            }

            //Disable motor
            PORTB |= 0x10;

            return 0;

        break;

        // Z
        case 2 :

            //Set Dir
            if (destination >= oldDestZ) // Go ascending Z, DirZ = 0 (OK)
            {
                PORTC |= 0x20;

                stepToDo = destination - oldDestZ;
            }

            else
            {
                PORTC &= 0x20;

                stepToDo = oldDestZ - destination;
            }

            // Remembering where we are
            oldDestZ = destination;

            //Enable motor
            PORTC &= 0x10; //Reversed logic on Enable pin

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if (!isContactTouched())
                {
                    PORTC |= 0x01; //High
                    delay_ms(delayStepZ);
                    PORTC &= 0x01; //Low
                    delay_ms(delayStepZ);
                }

                else
                    return whichContactTouched();
            }

            //Disable motor
            PORTC |= 0x10;

            return 0;

        break;

        //Rot Z
        case 3 :

            //Set Dir
            if (destination >= oldDestRotZ) // Go direct sense RotZ, DirRotZ = 0 (else 1 for Trigo)
            {
                PORTF |= 0x20;

                stepToDo = destination - oldDestRotZ;
            }

            else
            {
                PORTF &= 0x20;

                stepToDo = oldDestRotZ - destination;
            }

            // Remembering where we are
            oldDestRotZ = destination;

            //Enable motor
            PORTF &= 0x10; //Reversed logic on Enable pin

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
                if (!isContactTouched())
                {
                    PORTF |= 0x01; //High
                    delay_ms(delayStepRotZ);
                    PORTF &= 0x01; //Low
                    delay_ms(delayStepRotZ);
                }

                else
                    return whichContactTouched();
            }

            //Disable motor
            PORTF |= 0x10;

            return 0;

        break;

        //Error : unknown selected motor
        default :
            return 1;
    }
}

    //Set the delay between two steps (in ms)
uint8_t setSpeed(uint8_t selectedMotor, uint8_t delayStep)
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
            return 1;
    }

    return 0;
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
                    PORTA &= 0b00001110;
                    PORTL &= 0b00001110;
                break;

                case 1 :
                    PORTA |= 0b00000010;
                    PORTL |= 0b00000010;
                    PORTA &= 0b00001100;
                    PORTL &= 0b00001100;
                break;

                case 2 :
                    PORTA |= 0b00000100;
                    PORTL |= 0b00000100;
                    PORTA &= 0b00001010;
                    PORTL &= 0b00001010;
                break;

                case 3 :
                    PORTA |= 0b00001100;
                    PORTL |= 0b00001100;
                    PORTA &= 0b00000010;
                    PORTL &= 0b00000010;
                break;

                case 4 :
                    PORTA |= 0b00001110;
                    PORTL |= 0b00001110;
                break;

                default :
                    return 2; // Error : bad microStep factor
            }

        break;

        // Y
        case 1 :

            switch(microStep)
            {
                case 0 :
                    PORTB &= 0b00001110;
                break;

                case 1 :
                    PORTB |= 0b00000010;
                    PORTB &= 0b00001100;
                break;

                case 2 :
                    PORTB |= 0b00000100;
                    PORTB &= 0b00001010;
                break;

                case 3 :
                    PORTB |= 0b00001100;
                    PORTB &= 0b00000010;
                break;

                case 4 :
                    PORTB |= 0b00001110;
                break;

                default :
                    return 2; // Error : bad microStep factor
            }

        break;

        // Z
        case 2 :

            switch(microStep)
            {
                case 0 :
                    PORTC &= 0b00001110;
                break;

                case 1 :
                    PORTC |= 0b00000010;
                    PORTC &= 0b00001100;
                break;

                case 2 :
                    PORTC |= 0b00000100;
                    PORTC &= 0b00001010;
                break;

                case 3 :
                    PORTC |= 0b00001100;
                    PORTC &= 0b00000010;
                break;

                case 4 :
                    PORTC |= 0b00001110;
                break;

                default :
                    return 2; // Error : bad microStep factor
            }

            break;

        // RotZ
        case 3 :

            switch(microStep)
            {
                case 0 :
                    PORTF &= 0b00001110;
                break;

                case 1 :
                    PORTF |= 0b00000010;
                    PORTF &= 0b00001100;
                break;

                case 2 :
                    PORTF |= 0b00000100;
                    PORTF &= 0b00001010;
                break;

                case 3 :
                    PORTF |= 0b00001100;
                    PORTF &= 0b00000010;
                break;

                case 4 :
                    PORTF |= 0b00001110;
                break;

                default :
                    return 2; // Error : bad microStep factor
            }

            break;

            //Error : unknown selected motor
            default :
                return 1;
    }

    return 0;
}

    //Command the transistor commanding the pneumatic distributor
uint8_t setPump (bool onOff)
{
    if (onOff)
        PORTH |= 0x01;
    else
        PORTH &= 0x01;

    return 0;
}
