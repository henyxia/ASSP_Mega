// Includes
#include <avr/io.h>
#include <stdio.h>

// Functions
void ad_init(unsigned char channel)
{
    ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    ADMUX|=(1<<REFS0)|(1<<ADLAR);
    ADMUX=(ADMUX&0xf0)|channel;
    ADCSRA|=(1<<ADEN);
}

uint8_t ad_sample(void)
{
	ADCSRA|=(1<<ADSC);
	while(bit_is_set(ADCSRA, ADSC));
    return ADCH;
}

uint16_t analogRead(uint8_t pin)
{
        uint8_t low, high;

        if (pin >= 14)
            pin -= 14; // allow for channel (ex: A12) or pin numbers (ex : 12)

        //Enable the ADC
        ADCSRA|=(1<<ADEN);

        //Prescaler selection
        ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

        //Set the analog reference and selected pin on admux
        ADMUX|=(1<<REFS0);
        ADMUX |= (pin & 0x07);

        //Start the conversion
        ADCSRA|=(1<<ADSC);

        // ADSC is cleared when the conversion finishes
        while (bit_is_set(ADCSRA, ADSC));

        //Building the result on 10 bits
        low  = ADCL;
        high = ADCH;

        return (high << 8) | low;
}
