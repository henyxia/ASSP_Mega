//Includes
#include "vardelay.h"

// Function
void delay_ms (uint8_t delay)
{
    while(delay> 0)
    {
        _delay_ms(1);
        delay--;
    }
}
