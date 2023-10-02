#pragma once

#if defined(ST7796)

// Data specific to the ILI9341 controller
#define DISPLAY_SET_CURSOR_X 0x2A
#define DISPLAY_SET_CURSOR_Y 0x2B
#define DISPLAY_WRITE_PIXELS 0x2C

#if defined(ST7796)
#define DISPLAY_NATIVE_WIDTH 320
#define DISPLAY_NATIVE_HEIGHT 240
#else
#error Unknown display controller!
#endif

#define InitSPIDisplay InitST7796

void InitST7796(void);

void TurnDisplayOn(void);
void TurnDisplayOff(void);

#endif
