#include "SystemClockConfig.h"
#include <stm32f103xb.h>
#include <sys/_stdint.h>


extern void Default_Handler(void);

/**
 * \brief ����� ������ ���������� ����� ������� ��������������� ������ �������
 * ���������� �� 72 ��� �� �������� ������. ������ ��� ������������ �������� 
 * ������ � ���� ��������� �������� 360 ������.
 */ 
#define RCC_TIMEOUT	500


#ifndef F_CPU
/**
 * \brief ������� ����������, ��
 */ 
#define F_CPU			72000000UL			  
#endif


/**
 * \brief ����� ������ ���������� �� 1��
 */ 
#define SYSICK_1KHZ		(F_CPU/1000-1)

#define SET_PERIPH_BIT(addr) ((uint32_t *)((PERIPH_BB_BASE  + (addr-PERIPH_BASE)*32)))
#define RCC_CR_HSE_ON() SET_PERIPH_BIT(RCC_BASE)[RCC_CR_HSEON_Pos] = 1
#define RCC_CR_PLL_ON() SET_PERIPH_BIT(RCC_BASE)[RCC_CR_PLLON_Pos] = 1
#define SET_FLASH_LATHENCY_2() SET_PERIPH_BIT(FLASH_R_BASE)[1] = 1


/**
 * \brief ������� ���������� �� 72 ��� ���� �������� ���� �� ������� ����� 8 ���
 * � ��������� ���������� ���� �� 9. ��������! ����� ������������� �� ������������
 * �� ���� ���������� ���������, ��� ������� ������������ ���� APB1 �� ���������
 * 36 ��� (��� 72 ��� ���������� �������������� ��������� ������������ ���� APB1
 * �� 2, ������ ����� ����������� ������������ �� ����). ��������! ����� ��������������
 * ��������� ������� �� ���� ���������� ��������� �������� ������ �� Flash ������!
 * (���������� ��������� �������� �.3.3.3 RM0008, ��� 72 ��� ���������� ��������� 2
 * ����� ��������). ������������� 2 ����� �������� ��� ������ �� Flash � �����������
 * ������������ SYSCLK �� ����.
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
 * \brief ����������� ��������� ������, ����� ����� ������ 1 ��. ��� ����� ����� ����������
 * � ������� ������� �������� F_CPU/1000, ����� ��������� ������������ �������,  ���������
 * ���������� ����� ������� ��������� �� ���� � ��������� ������. ����� ������������� ���������
 * �� ���������� � ��������� ���������� �� ���������� �������.
 */ 
void InitSysTick()
{
	SysTick->LOAD = SYSICK_1KHZ;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;
	NVIC_SetPriority(SysTick_IRQn, 15);
	NVIC_EnableIRQ(SysTick_IRQn);
}