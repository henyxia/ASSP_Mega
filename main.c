// Includes
#include <avr/io.h>
#include <util/delay.h>

#include "serial.h"
#include "analog.h"
#include "functions.h"
#include "vardelay.h"

// Defines
#define MAJOR_VERSION	0
#define	MINOR_VERSION	1

// Globals
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

    //Initialize position (X=0, Y=0)
    init_position();

    //Initialize position for needle (Z axis)
    init_position_Z();

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

            //setMotLocked called
        case 0x06 :
        {
            //OK, roger that, deducting orders and perform setMotLocked
            returnCode = setMotLocked((serFrame1 & 0x03),(serFrame1 & 0x04));
            send_serial(returnCode);
        }
            break;

            //getMotLocked called
        case 0x07 :
        {
            send_serial(getMotLocked() << 4);
        }
            break;

        //getDest called
        case 0x08 :
        {
            destination = getDest(serFrame1);

            //OK, roger that, let's send first frame if correct mot
            if (destination != MOT_NOT_KNOWN_GET)
            {
                //First frame : OK
                send_serial(WAIT_FOR_IT_2);
                get_serial();
                //Frame 2 : dest[15:8]
                send_serial(destination >> 8);
                get_serial();
                //Frame 3 : dest[7:0]
                send_serial(destination & 0x00FF);
            }

            //Bad mot
            else
            {
                send_serial(MOT_NOT_KNOWN);
            }
        }
            break;

    //getMS called
        case 0x09 :
        {
            destination = getMS();

            send_serial(((destination & 0x0F00) >> 4) |WAIT_FOR_IT_1);
            get_serial();
            send_serial(destination & 0x00FF);
        }
            break;

        //getPump called
        case 0x0A :
        {
            send_serial(getPump() << 4);
        }
            break;

        //getADC called
        case 0x0B :
        {
            destination = getADC();

            send_serial(((destination & 0x0300) >> 4) | WAIT_FOR_IT_1);
            get_serial();
            send_serial(destination & 0x00FF);
        }
            break;

        //getRelease called
        case 0x0C :
        {
            //TO DO
        }
            break;

        //getADCValue called
        case 0x0D :
        {
            destination = getADCvalue(serFrame1);

            send_serial(((destination & 0x0300) >> 4) | WAIT_FOR_IT_1);
            get_serial();
            send_serial(destination & 0x00FF);
        }
            break;

        //getSpeed called
        case 0x0E :
        {
            returnCode = getSpeed(serFrame1);

            //OK, roger that, let's send first frame if correct mot
            if (returnCode != 0)
            {
                //First frame : OK
                send_serial(WAIT_FOR_IT_1);
                get_serial();
                //Frame 2 : delayStep wanted
                send_serial(returnCode);
            }

            else
                send_serial(MOT_NOT_KNOWN);
        }
            break;

        //getVersion called
        case 0x0F:
            // Asking for the version code
            send_serial((MINOR_VERSION << 4) | WAIT_FOR_IT_1);
            get_serial();
            send_serial(MAJOR_VERSION);
            break;

            //Called command not known
        default :
            send_serial(CMD_NOT_KNOWN);
        }
    }

	return 0;
}
