#pragma once
#include <stm32f103xb.h>
#include <sys/_stdint.h>

/**
 * \brief todo: �������� ��������
 */
typedef enum
{
	DmaModePeriphToMem = 0 << 4,
	DmaModeMemToPeriph = 1 << 4,
	DmaModeMemToMem = 1 << 14
}DmaMode;

/**
 * \brief todo: �������� ��������
 */
typedef enum
{
	DmaPriorityLow      = 0 << 12,
	DmaPriorityMedium   = 1 << 12,
	DmaPriorityHigh     = 2 << 12,
	DmaPriorityVeryHigh = 3 << 12
}DmaPriority;

/**
 * \brief todo: �������� ��������
 */
typedef enum
{
	DmaMemorySize8bit   = 0 << 10,
	DmaMemorySize16bits = 1 << 10,
	DmaMemorySize32bits = 2 << 10
}DmaMemorySize;

/**
 * \brief todo: �������� ��������
 */
typedef enum
{
	DmaPeripheralSize8bit   = 0 << 8,
	DmaPeripheralSize16bits = 1 << 8,
	DmaPeripheralSize32bits = 2 << 8
}DmaPeripheralSize;

/**
 * \brief todo: �������� ��������
 */
typedef enum
{
	DmaSizeConfMem8bitPeriph8bit     = 0 << 10 | 0 << 8,
	DmaSizeConfMem8bitPeriph16bits   = 0 << 10 | 1 << 8,
	DmaSizeConfMem8bitPeriph32bits   = 0 << 10 | 2 << 8,

	DmaSizeConfMem16bitsPeriph8bit   = 1 << 10 | 0 << 8,
	DmaSizeConfMem16bitsPeriph16bits = 1 << 10 | 1 << 8,
	DmaSizeConfMem16bitsPeriph32bits = 1 << 10 | 2 << 8,

	DmaSizeConfMem32bitsPeriph8bit   = 2 << 10 | 0 << 8,
	DmaSizeConfMem32bitsPeriph16bits = 2 << 10 | 1 << 8,
	DmaSizeConfMem32bitsPeriph32bits = 2 << 10 | 2 << 8
}DmaSizeConf;

/**
 * \brief ����� �������������� ������� �������� � ����������� (�� �.13.4.3)
 */
typedef enum
{
	DmaIncrementModeIncOff          = 0 << 6,
	DmaIncrementModeIncPeriph       = 1 << 6,
	DmaIncrementModeIncMem          = 2 << 6,
	DmaIncrementModeIncMemAndPeriph = 3 << 6
}DmaIncrementMode;

/**
 * \brief ����� DMA, ��� ��������������� �� ����� ������ CMSIS.
 */
typedef DMA_Channel_TypeDef DmaChannel;

/**
 * \brief ������� ��������� ������ DMA.
 * \param dma ����� DMA, ������� ����� �����������.
 * \param mode ����� �������� ������.
 * \param priority ��������� ������.
 * \param size ������ ������ �������� � �����������.
 * \param incMode ��������� �������������� �������.
 */
inline void DmaConfigureChannel(DmaChannel* dma, DmaMode mode, DmaPriority priority, DmaSizeConf size, DmaIncrementMode incMode)
{
	dma->CCR &= ~0x1FFF;
	dma->CCR |= mode | priority | size | incMode;
}

/**
 * \brief todo: �������� ��������
 * \param dma 
 * \param addr 
 */
inline void DmaSetPeripheralRegister(DmaChannel* dma, uint32_t addr)
{
	dma->CPAR = (uint32_t)&addr;
}

/**
 * \brief ��������� ������������ ����� DMA1
 */
inline void DmaClockEnable()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}