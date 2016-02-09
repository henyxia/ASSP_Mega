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
        serFunctionCalled = serFrame & 0xF0; //Getting b3:b0
        serFrame1 = serFrame >> 4; //Getting b7:b4

        switch(serFunctionCalled)
        {

        //setDest called
        case 0x00 :
        {
            //Deducting selectedMotor frame
            serFrame1 = serFrame1 >> 2;

            send_serial(0x00); // OK, roger that, waiting for dest

            //Waiting for destination frames
            serFrame2 = get_serial();
            send_serial(0x00); // LINE TO VERIFY
            serFrame3 = get_serial();

            //Deducting destination
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
            //Deducting selectedMotor frame
            serFrame1 = serFrame1 >> 2;

            send_serial(0x00); // OK, roger that, waiting for MS

            //Waiting for MS frame
            serFrame2 = get_serial();

            //Deducting correct MS parameter
            serFrame2 = serFrame2 >> 5;

            //Perform setMS
            returnCode = setMS(serFrame1, serFrame2);
            send_serial(returnCode);
        }
            break;

        //setPump called
        case 0x02 :
        {
            //send_serial(0x00); // TO VERIFY : OK, command known

            //Perform setPump
            returnCode = setPump(serFrame1);
            send_serial(returnCode);
        }
            break;

        //setADC called
        case 0x03 :
        {
            send_serial(0x00); // OK, roger that, waiting for next frame

            //Waiting for second adcLevel frame
            serFrame2 = get_serial();

            //Deducting correct adcLevel wanted
            serFrame1 = serFrame1 >> 2;
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
        case 0x0D :
        {
            //Deducting selectedMotor frame
            serFrame1 = serFrame1 >> 2;

            send_serial(0x00); // OK, roger that, waiting for delayStep

            //Waiting for delayStep frame
            serFrame2 = get_serial();

            //Perform setSpeed
            returnCode = setSpeed(serFrame1, serFrame2);
            send_serial(returnCode);
        }

        //Called command not known
        default :
            send_serial(0x01);
        }

    }

	return 0;
}
