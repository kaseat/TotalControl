#include "SystemClockConfig.h"
#include <stm32f103xb.h>
#include <sys/_stdint.h>


extern void Default_Handler(void);

/**
 * \brief Число циклов процессора после которых диагностируется ошибка запуска
 * процессора на 72 МГц от внешнего кварца. Обычно для стабилизации внешнего 
 * кварца и ФАПЧ требуется примерно 360 циклов.
 */ 
#define RCC_TIMEOUT	500


#ifndef F_CPU
/**
 * \brief Частота процессора, Гц
 */ 
#define F_CPU			72000000UL			  
#endif


/**
 * \brief Число тактов процессора за 1мс
 */ 
#define SYSICK_1KHZ		(F_CPU/1000-1)

#define SET_PERIPH_BIT(addr) ((uint32_t *)((PERIPH_BB_BASE  + (addr-PERIPH_BASE)*32)))
#define RCC_CR_HSE_ON() SET_PERIPH_BIT(RCC_BASE)[RCC_CR_HSEON_Pos] = 1
#define RCC_CR_PLL_ON() SET_PERIPH_BIT(RCC_BASE)[RCC_CR_PLLON_Pos] = 1
#define SET_FLASH_LATHENCY_2() SET_PERIPH_BIT(FLASH_R_BASE)[1] = 1


/**
 * \brief Выводим контроллер на 72 МГц путём перевода ядра на внешний кварц 8 МГц
 * и настройки умножитель ФАПЧ на 9. Внимание! Перед переключением на тактирование
 * от ФАПЧ необходимо убедиться, что частота тактирования шины APB1 не превышает
 * 36 МГц (для 72 МГц необходимо предварительно выставить предделитель шины APB1
 * на 2, только затем переключать тактирование на ФАПЧ). Внимание! Перед пререключением
 * тактового сигнала на ФАПЧ необходимо настроить задержку чтения из Flash памяти!
 * (Необходимо настроить согласно п.3.3.3 RM0008, для 72 МГц необходимо выставить 2
 * цикла задержки). Устанавливаем 2 цикла задержки для чтения из Flash и переключаем
 * тактирование SYSCLK на ФАПЧ.
 */ 
void InitSystemClock()
{
	uint32_t tickCounter = 0;

	RCC_CR_HSE_ON();
	while(!(RCC->CR & RCC_CR_HSERDY))
		if(++tickCounter > RCC_TIMEOUT)
			Default_Handler();
	
	RCC->CFGR = RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

	RCC_CR_PLL_ON();
	while(!(RCC->CR & RCC_CR_PLLRDY))
		if(++tickCounter >  RCC_TIMEOUT)
			Default_Handler();

	SET_FLASH_LATHENCY_2();
	RCC->CFGR = RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_SW_PLL;
}


/**
 * \brief Настраиваем системный таймер, чтобы тикал каждые 1 мс. Для этого нужно установить
 * в счётный регистр значение F_CPU/1000, затем отключить предделитель частоты,  разрешить
 * прерывание когда счётчик досчитает до ноля и запустить таймер. Затем устанавливаем приоритет
 * на прерывания и разрешаем прерывание от системного таймера.
 */ 
void InitSysTick()
{
	SysTick->LOAD = SYSICK_1KHZ;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;
	NVIC_SetPriority(SysTick_IRQn, 15);
	NVIC_EnableIRQ(SysTick_IRQn);
}