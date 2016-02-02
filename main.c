// Includes
#include <avr/io.h>
#include <util/delay.h>

#include "serial.h"
#include "analog.h"
#include "functions.h"
#include "vardelay.h"

//Globals


// Main
int main(void)
{
    // Initialization
    init_port();
    init_serial(9600);

	return 0;
}
