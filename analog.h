// Protector
#ifndef __ANALOG__
#define __ANALOG__

// Functions
void ad_init(unsigned char channel);
uint8_t ad_sample(void);
uint16_t analogRead(uint8_t pin);

#endif
