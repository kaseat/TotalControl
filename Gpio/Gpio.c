#include "Gpio.h"
#include <sys/_stdint.h>


/**
 * \brief todo: добавить описание!
 * \param port 
 * \param pin 
 * \param mode 
 */ 
void GpioPinConfig(GpioPort* port, GpioPin pin, GpioMode mode)
{

#if FORCE_CLK !=0
	GpioPortClockInit(port);		  
#endif

	uint32_t  mask = mode & 0xFU;
	if (pin < GpioPin8)
	{
		port->CRL &= ~(0xF << pin * 4);
		port->CRL |= mask << pin * 4;
	}
	else
	{
		port->CRH &= ~(0xF << (pin - 8) * 4);
		port->CRH |= mask << (pin - 8) * 4;
	}

	port->ODR |= mode >> 4 << pin;
}
