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

#define DMA_TX 1
#define DMA_RX 0


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
void Uart1Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
	USART1->CR1 &= ~USART_CR1_UE;
	GpioPinConfig(USART1_PORT, USART1_TX_PIN, USART1_TX_MODE);
	GpioPinConfig(USART1_PORT, USART1_RX_PIN, USART1_RX_MODE);
	USART1->BRR = F_CPU / USART1_BAUDRATE;
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR;
	Dma1ClockEnable();
	DmaConfigureChannel(DMA1_Channel4, DmaModeMemToPeriph, DmaPriorityMedium, DmaSizeConfMem8bitPeriph8bit, DmaIncrementModeIncMem);
	DmaSetPeripheralRegister(DMA1_Channel4, &USART1->DR);
	DmaConfigureChannel(DMA1_Channel5, DmaModePeriphToMem, DmaPriorityHigh, DmaSizeConfMem8bitPeriph8bit, DmaIncrementModeIncMem);
	DmaSetPeripheralRegister(DMA1_Channel5, &USART1->DR);
}