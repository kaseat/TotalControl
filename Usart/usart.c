#include "usart.h"
#include "Drivers/Gpio/Gpio.h"
#include "Drivers/Dma/Dma.h"

#ifndef F_CPU
#define F_CPU 72000000UL
#endif

#define USART1_TX_PIN 9
#define USART1_RX_PIN 10
#define USART1_TX_MODE GpioModeAfoPushPullMidSpeed
#define USART1_RX_MODE GpioModeInputFloating
#define USART1_PORT GPIOA
#define USART1_BAUDRATE 115200

#define DMA_TX 0
#define DMA_RX 1
extern void Default_Handler(void);


/**
 * \brief Инициализация UART.
 * \note Сначала необходимо включить тактирование порта, затем убедиться что UART отключен,
 * Затем настраисаем пины - пин TX (9) как альтернативный вывод push-pull (таблица 24 RM0008),
 * пин RX (10) как вход с открытым коллектором (таблица 24 RM0008), затем настраиваем зкорость,
 * и разрешаем работу UART, приёмника и передатчика.
 * Если используем DMA, то разрешаем его в настройках UART, пишем адрес регистра DR (UART) в
 * настройки DMA, очищаем настройки DMA  и настраиваем DMA как автоинкремент в памяти  с
 * направлением память->устройство (п. 13.3.3 RM0008)
 */ 
void UartInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
	USART1->CR1 &= ~USART_CR1_UE;
	GpioPinConfig(USART1_PORT, USART1_TX_PIN, USART1_TX_MODE);
	GpioPinConfig(USART1_PORT, USART1_RX_PIN, USART1_RX_MODE);
	USART1->BRR = F_CPU / USART1_BAUDRATE;
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

#if DMA_TX != 0
	USART1->CR3 |= USART_CR3_DMAT;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
	DMA1_Channel4->CCR &= ~0x1FFF;
	DMA1_Channel4->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
#endif

#if DMA_RX != 0
	USART1->CR3 |= USART_CR3_DMAR;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	//DMA1_Channel5->CPAR = (uint32_t)&USART1->DR;
//	DMA1_Channel5->CCR &= ~0x1FFF;
//	DMA1_Channel5->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
#endif
	DmaConfigureChannel(DMA1_Channel5, DmaModePeriphToMem, DmaPriorityHigh,DmaSizeConfMem8bitPeriph8bit, DmaIncrementModeIncMem);
	DmaSetPeripheralRegister(DMA1_Channel5, USART1->DR);
	DmaClockEnable(DMA1_Channel5);
}
