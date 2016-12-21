#pragma once
#include <stm32f103xb.h>
#include <sys/_stdint.h>


/**
 * \brief Принудительно включать тактирование порта перед изменением
 * конфигурации пина, если FORCE_CLK не равен нолю.
 */ 
#define FORCE_CLK 0


/*
 * \brief Нумерация пинов.
 */ 
typedef enum
{
	GpioPin0,
	GpioPin1,
	GpioPin2,
	GpioPin3,
	GpioPin4,
	GpioPin5,
	GpioPin6,
	GpioPin7,
	GpioPin8,
	GpioPin9,
	GpioPin10,
	GpioPin11,
	GpioPin12,
	GpioPin13,
	GpioPin14,
	GpioPin15
} GpioPin;


/**
 * \brief Режимы работы пина.
 */ 
typedef enum
{
	GpioModeGpoPushPullLowSpeed   = 0b00010U,
	GpioModeGpoPushPullMidSpeed   = 0b00001U,
	GpioModeGpoPushPullHighSpeed  = 0b00011U,

	GpioModeGpoOpenDrainLowSpeed  = 0b00110U,
	GpioModeGpoOpenDrainMidSpeed  = 0b00101U,
	GpioModeGpoOpenDrainHighSpeed = 0b00111U,

	GpioModeAfoPushPullLowSpeed   = 0b01010U,
	GpioModeAfoPushPullMidSpeed   = 0b01001U,
	GpioModeAfoPushPullHighSpeed  = 0b01011U,

	GpioModeAfoOpenDrainLowSpeed  = 0b01110U,
	GpioModeAfoOpenDrainMidSpeed  = 0b01101U,
	GpioModeAfoOpenDrainHighSpeed = 0b01111U,

	GpioModeInputAnalog           = 0b00000U,
	GpioModeInputFloating         = 0b00100U,
	GpioModeInputPullDown         = 0b01000U,
	GpioModeInputPullUp           = 0b11000U,
}GpioMode;


/**
 * \brief Значение на пине (высокий или низкий сигнал)
 */ 
typedef enum
{
	GpioValueLo = 1,
	GpioValueHi = 0
} GpioValue;


/**
 * \brief Унифицируем библиотеку путём абстакции от CMSIS
 */ 
typedef GPIO_TypeDef GpioPort;


/**
 * \brief Функция для конфигурации пина.
 * \param port Порт, на котром висит пин.
 * \param pin Номер пина, который конфигурируем.
 * \param mode Режим, в который переводим наш пин.
 */ 
void GpioPinConfig(GpioPort* port, GpioPin pin, GpioMode mode);

/**
 * \brief Функция для конфигурации пина (встраивается вместо вызова,
 * что экономит время, но расходует место).
 * \param port Порт, на котром висит пин.
 * \param pin Номер пина, который конфигурируем.
 * \param mode Режим, в который переводим наш пин.
 */ 
inline void GpioPinConfigInline(GpioPort* port, GpioPin pin, GpioMode mode)
{

	if (pin < GpioPin8)
	{
		port->CRL &= ~(0xF << pin * 4);
		port->CRL |= mode & 0xFU << pin * 4;
	}
	else
	{
		port->CRH &= ~(0xF << (pin - 8) * 4);
		port->CRH |= mode & 0xFU << (pin - 8) * 4;
	}

	port->ODR |= mode >> 4 << pin;
}


/**
 * \brief Функция для инициализации тактирования порта.
 * \param port Порт, тактирование которого включаем.
 * \note Здесь мы просто переводим значение указателя на порт в маску
 * для регистра APB2ENR. Значения указателей на порт различаются на 0x400
 * поэтому можно посчитать номер порта относительно указателя на GPIOA
 * по формуле (GPIOx-GPIOA)/0x400. Так мы получим смещение относительно
 * порта А. Смещение бита реазрешения тактирования порта А в регистре
 * APB2ENR равно 2 (п.7.3.7 RM0008), поэтому прибавив к нашему
 * числу 2, получим необходимое нам смещение. Далее загоняем этот бит в
 * регистр APB2ENR через bit band. В итоге получаем функцию, которая выполняется за 3
 * инструкции процессора и занимает 6 байт памяти.
 */ 
inline void GpioPortClockInit(GpioPort* port)
{
	((uint32_t*)(PERIPH_BB_BASE + ((uint32_t)&RCC->APB2ENR - PERIPH_BASE) * 32))[(port - GPIOA) / 0x400 + 2] = 1;
}

/**
 * \brief Функция для записи в пин 0 или 1.
 * \param port Порт, на котром висит пин.
 * \param pin Пин, значение которого устанавливаем.
 * \param value Собственно, само значение.
 * \note Исполняется за две 16-ти битных инструкции 
 * процессора -> 4 байта - это меньше чем банальный
 * вызов функции (8 байт) и гораздо быстрее.
 */ 
inline void GpioPinWrite(GpioPort* port, GpioPin pin, GpioValue value)
{
	port->BSRR = 1 << (pin + 16*value);
}


/**
 * \brief Функция для инверсии сигнала на пине ().
 * \param port Порт, на котром висит пин.
 * \param pin Пин, значение которого иныертируем.
 * \note Исполняется за три инструкции процессора: 
 * две 16-разрядных и одна 32-х, итогом 8 байт, что
 * по размеру равно вызову функции, а по скорости
 * быстрее.
 */ 
inline void GpioPinInverse(GpioPort* port, GpioPin pin)
{
	port->ODR ^= 1 << pin;
}