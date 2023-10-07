#include "config.h"

#if defined(ST7796)

#include "spi.h"

#include <memory.h>
#include <stdio.h>

void InitST7796()
{
  // If a Reset pin is defined, toggle it briefly high->low->high to enable the device. Some devices do not have a reset pin, in which case compile with GPIO_TFT_RESET_PIN left undefined.
#if defined(GPIO_TFT_RESET_PIN) && GPIO_TFT_RESET_PIN >= 0
  printf("Resetting display at reset GPIO pin %d\n", GPIO_TFT_RESET_PIN);
  SET_GPIO_MODE(GPIO_TFT_RESET_PIN, 1);
  SET_GPIO(GPIO_TFT_RESET_PIN);
  usleep(120 * 1000);
  CLEAR_GPIO(GPIO_TFT_RESET_PIN);
  usleep(120 * 1000);
  SET_GPIO(GPIO_TFT_RESET_PIN);
  usleep(120 * 1000);
#endif

  // Do the initialization with a very low SPI bus speed, so that it will succeed even if the bus speed chosen by the user is too high.
  spi->clk = 34;
  __sync_synchronize();

  BEGIN_SPI_COMMUNICATION();
  {

#define MADCTL_BGR_PIXEL_ORDER (1<<3)
#define MADCTL_ROW_COLUMN_EXCHANGE (1<<5)
#define MADCTL_COLUMN_ADDRESS_ORDER_SWAP (1<<6)
#define MADCTL_ROW_ADDRESS_ORDER_SWAP (1<<7)
#define MADCTL_ROTATE_180_DEGREES (MADCTL_COLUMN_ADDRESS_ORDER_SWAP | MADCTL_ROW_ADDRESS_ORDER_SWAP)

    uint8_t madctl = 0;

#ifdef DISPLAY_ROTATE_180_DEGREES
    madctl ^= MADCTL_ROTATE_180_DEGREES;
#endif

	// ST7796 SPI init sequence
	//
	// Taken from Anycubic Kobra Neo Marlin fork BSP source: 
	// https://github.com/jokubasver/Kobra_Neo/blob/d3406176308f1839130edc08825f500a72c02f64/source/board/bsp_spi_tft.cpp#L406C12-L406C12
    SPI_TRANSFER(0x11);
    usleep(120*1000);
    SPI_TRANSFER(0x29);
    SPI_TRANSFER(0x36, 0xb0);
    SPI_TRANSFER(0x3a, 0x5);
    SPI_TRANSFER(0x13);
    SPI_TRANSFER(0xb2, 0x05, 0x05, 0x00, 0x33, 0x33);
    SPI_TRANSFER(0xb7, 0x35);
    SPI_TRANSFER(0xBb, 0x19);
    SPI_TRANSFER(0xc0, 0x2c);
    SPI_TRANSFER(0xc2, 0x01);
    SPI_TRANSFER(0xc3, 0x13);
    SPI_TRANSFER(0xc4, 0x20);
    SPI_TRANSFER(0xc6, 0x0f);
    SPI_TRANSFER(0xd0, 0xa4, 0xa1);
    SPI_TRANSFER(0xe0, 0xd0, 0x04, 0x0d, 0x11, 0x13, 0x2b, 0x3f, 0x54, 0x4c, 0x18, 0x0d, 0x0b, 0x1f, 0x23);
    SPI_TRANSFER(0xe1, 0xd0, 0x04, 0x0c, 0x11, 0x13, 0x2c, 0x3f, 0x44, 0x51, 0x2f, 0x1f, 0x1f, 0x20, 0x23);
    SPI_TRANSFER(0x29);
    usleep(100 * 1000);

    ClearScreen();
  }
#ifndef USE_DMA_TRANSFERS // For DMA transfers, keep SPI CS & TA active.
  END_SPI_COMMUNICATION();
#endif

  // And speed up to the desired operation speed finally after init is done.
  usleep(10 * 1000); // Delay a bit before restoring CLK, or otherwise this has been observed to cause the display not init if done back to back after the clear operation above.
  spi->clk = SPI_BUS_CLOCK_DIVISOR;
}

void TurnDisplayOff()
{
#if 0
  QUEUE_SPI_TRANSFER(0x28/*Display OFF*/);
  QUEUE_SPI_TRANSFER(0x10/*Enter Sleep Mode*/);
  usleep(120*1000); // Sleep off can be sent 120msecs after entering sleep mode the earliest, so synchronously sleep here for that duration to be safe.
#endif
//  printf("Turned display OFF\n");
}

void TurnDisplayOn()
{
#if 0
  QUEUE_SPI_TRANSFER(0x11/*Sleep Out*/);
  usleep(120 * 1000);
  QUEUE_SPI_TRANSFER(0x29/*Display ON*/);
#endif
//  printf("Turned display ON\n");
}

void DeinitSPIDisplay()
{
  ClearScreen();
}

#endif
