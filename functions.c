/* Pinout Arduino :

 * Motor X1 : Port A
 * Motor X2 : Port L
 * Motor Y : Port B
 * Motor Z : Port C
 * Motor RotZ : Port F

 * Bit Port :   7   |   6   |   5    |     4     ||     3    |    2    |    1    |   0
 * Bit Driv :       |        |   Dir | ~Enab ||  MS3  | MS2 |  MS1 | Step


 *  Pump : Port D (D0)

*/

// Includes
#include "functions.h"

//Globals
uint16_t oldDestX, oldDestY, oldDestZ, oldDestRotZ = 0;
uint8_t MSX, MSY, MSZ, MSRotZ = 1;
uint8_t delayStepX, delayStepY, delayStepZ, delayStepRotZ = 30;

// Functions

    // Init motor ports
void init_port(void)
{
    // Init pinmode (as outputs)
    DDRA = 0b00111111;
    DDRL = 0b00111111;
    DDRB = 0b00111111;
    DDRC = 0b00111111;
    DDRF = 0b00111111;

    DDRD = 0x01;

    // Init values (Drivers disabled by default at init, and reversed logic on Enable pin)
    PORTA = 0b00010000;
    PORTL = 0b00010000;
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

            // Remembering where we are
            oldDestX = destination;

            //Enable motors
            PORTA &= 0x10; //Reversed logic on Enable pin
            PORTL &= 0x10;

            // Perform steps
            for (i = 0; i < stepToDo; i++)
            {
               PORTA |= 0x01; //High
               PORTL |= 0x01;
               delay_ms(delayStepX);
               PORTA &= 0x01; //Low
               PORTL &= 0x01;
               delay_ms(delayStepX);
            }

            //Disable motors
            PORTA |= 0x10;
            PORTL |= 0x10;

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
                PORTB |= 0x01; //High
               delay_ms(delayStepY);
               PORTB &= 0x01; //Low
               delay_ms(delayStepY);
            }

            //Disable motors
            PORTB |= 0x10;

        break;

        // Z
        case 2 :

        break;

        //Rot Z
        case 3 :

        break;

        //Error
        default :

        return 12;
    }

}

    //Set the delay between two steps (in ms)
uint8_t setSpeed(uint8_t selectedMotor, uint8_t delayStep)
{
    switch(selectedMotor)
    {
        // X
        case 0 :

        break;

        // Y
        case 1 :

        break;

        // Z
        case 2 :

        break;

        //Rot Z
        case 3 :

        break;

        //Error
        default :

        return 12;
    }
}

    //Set the micro-step factor of the selected motor
uint8_t setMS (uint8_t selectedMotor, uint8_t microStep)
{
    switch(selectedMotor)
    {
        // X
        case 0 :

        break;

        // Y
        case 1 :

        break;

        // Z
        case 2 :

        break;

        //Rot Z
        case 3 :

        break;

        //Error
        default :

        return 12;
    }
}

    //Command the pneumatic distributor
uint8_t setPump (bool onOff)
{

}
