// Includes
#include <avr/io.h>
#include <util/delay.h>

#include "serial.h"
#include "analog.h"
#include "functions.h"
#include "vardelay.h"

//Globals
uint8_t serFrame;
uint8_t serFunctionCalled = 0;
uint8_t serFrame1, serFrame2, serFrame3 = 0;
uint8_t returnCode = 0;

uint16_t destination = 0;

// Main
int main(void)
{
    // Initialization
    init_port();
    init_serial(9600);

    //Reading serial and reacting
    while (1)
    {
        //Getting first frame
        serFrame = get_serial();
        serFunctionCalled = serFrame & 0x0F; //Getting b3:b0
        serFrame1 = serFrame >> 4; //Getting b7:b4

        switch(serFunctionCalled)
        {

        //setDest called
        case 0x00 :
        {
             // OK, roger that, waiting for dest
            send_serial(WAIT_FOR_NEXT_FRAME);

            //Waiting for destination frames
            serFrame2 = get_serial();
            send_serial(WAIT_FOR_NEXT_FRAME);
            serFrame3 = get_serial();

            //Deducting destination
            destination = 0;
            destination = serFrame2 << 8;
            destination |= serFrame3;

            //Perform setDest and return the associated code
            returnCode = setDest(serFrame1, destination);
            send_serial(returnCode);
        }
            break;

        //setMS called
        case 0x01 :
        {
            // OK, roger that, waiting for MS
            send_serial(WAIT_FOR_NEXT_FRAME);

            //Waiting for MS frame
            serFrame2 = get_serial();

            //Perform setMS
            returnCode = setMS(serFrame1, serFrame2);
            send_serial(returnCode);
        }
            break;

        //setPump called
        case 0x02 :
        {
            //Perform setPump
            returnCode = setPump(serFrame1);
            send_serial(returnCode);
        }
            break;

        //setADC called
        case 0x03 :
        {
            // OK, roger that, waiting for next frame
            send_serial(WAIT_FOR_NEXT_FRAME);

            //Waiting for second adcLevel frame
            serFrame2 = get_serial();

            //Deducting correct adcLevel wanted
            destination = 0;
            destination = serFrame1 << 8;
            destination |= serFrame2;

            //Perform setDest and return the associated code
            returnCode = setADC(destination);
            send_serial(returnCode);
        }
            break;

        //setRelease called
        case 0x04 :
        {
            // TO DO
        }
            break;

        //setSpeed called
        case 0x05 :
        {
            // OK, roger that, waiting for delayStep
            send_serial(WAIT_FOR_NEXT_FRAME);

            //Waiting for delayStep frame
            serFrame2 = get_serial();

            //Perform setSpeed
            returnCode = setSpeed(serFrame1, serFrame2);
            send_serial(returnCode);
        }
            break;

        //getDest called
        case 0x08 :
        {
            // TO DO
        }
            break;

        //Called command not known
        default :
            send_serial(CMD_NOT_KNOWN);
        }

    }

	return 0;
}
