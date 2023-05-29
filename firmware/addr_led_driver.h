#ifndef ADDR_LED_DRIVER_H_
#define ADDR_LED_DRIVER_H_

#include <stdlib.h>
#include <stdint.h>
#include "hardware/timer.h"

#define ADDR_LED_SIGNAL_GPIO_PIN 0 //16

#define ADDR_LED_PWM_DIVIDER 20
#define ADDR_LED_PWM_WRAP 6 // eyeball measure. leds seem fine with it
#define ADDR_LED_PWM_HI_CC_VAL 4
#define ADDR_LED_PWM_LO_CC_VAL 2

#define ADDR_LED_UPDATE_PERIOD_MS 100

#define NUM_LEDS_PER_PANEL (16)
#define NUM_LEDS_PER_PANEL_SIDE (4)
#define NUM_PANELS (5)
#define NUM_LEDS (NUM_LEDS_PER_PANEL * NUM_PANELS)
#define SIGNAL_BUFFER_LEN (sizeof(PixelPacket_t) * NUM_LEDS + 1)

// Sides enum
typedef enum {
  NORTH,
  EAST,
  SOUTH,
  WEST,
  TOP,
  NUM_SIDES 
} Position_e;

// This driver mainly aims to drive a strip of neopixels. on top of that, it will have some low level data structures and definitions that implement the specific shape/layout of my strip, which is a cube; 5 sides that are 4x4 neopixel panels, made up of a single continuous strip. 

// LEDs are arranged in a 1 dimensional array, even though they are physically arranged in a square shape (or else)
// Below structure describes that. we'll see if i keep it...
// I realize a Panel may be in the middle or in some arbitrary location of the LED strip/chain. To take that into account
// the field $stripRange is there. $stripRange[0] - $stripRange[1] denote the beginning and ending LED indices that correspond
// to the beginning and the ending LEDs in the strip
// Example:
//
//              Panel 0            Panel 1             ..
//              ____________       _______________
//        IN-> |0  1  2  3  |  _> |16  17  18  19 |  _> ...
//             |4  5  6  7  | |   |20  21  22  23 | |
//             |8  9  10 11 | |   |24  25  26  27 | |
//             |12 13 14 15 |/    |28  29  30  31 |/
//     
// stripRange      0 - 15           16 - 31
//
// Meaning that the 0th pixel of Panel 1 is actually LED 16 on the strip/chain.
//
// Panels have "Position" and "Neighbors". Below is how the current plan of panel layout looks like:
//
//              [ N ]
//        [ W ] [ C ] [ E ]
//              [ S ]
// 
// This is how the cube of panels will be imagined in 2d. 
// Neighboring panels are obvious; like the neighbors of N are W, C, E. Center panel's neighbors are all other panels, etc. 
//
// Panel LED coordinates:
// 0,3 1,3 2,3 3,3
// 0,2 1,2 2,2 3,2
// 0,1 1,1 2,1 3,1
// 0,0 1,0 2,0 3,0
//
// Y = 0 when on the bottom.

// Below denotes the raw datastructure that will be sent to the the WS2812B via DMA.
// Each uint8_t represents the duty cycle (CCR value) the PWM will be set to.
typedef struct {
  uint16_t greenRaw[8];
  uint16_t redRaw[8];
  uint16_t blueRaw[8];
} __attribute__((packed)) PixelPacket_t;

// Below type denotes a single pixel (1 LED)
// Some fields may be redundant
typedef struct {
  uint8_t green;
  uint8_t red;
  uint8_t blue;
  uint8_t x; // X, Y of the pixel IN it's panel // todo think about moving these to manager
  uint8_t y;
  uint8_t globalX;
  uint8_t globalY;
  uint8_t globalZ;

  //Pixel_t *pixN; // The pixel to the North, East, South, West of this pixel
  //Pixel_t *pixE;
  //Pixel_t *pixS;
  //Pixel_t *PixW; // TODO
} Pixel_t;

// Below structure denotes the encapsulation of one continuous LED strip (can be in any form. location of pixels handled by upper layer)
typedef uint16_t PixelPacketBuffer_t;
typedef struct {
  uint16_t numLeds; // Number of LEDs in the strip
  Pixel_t *pixels; // The 1 dimensional array of Pixel_t objects. an upper layer manages the locations of said pixels
  PixelPacketBuffer_t *pixelPacketBuffer; // This is the bufer that will hold the raw data bytes to be transmitted that corresponds to the $*pixels array 
} AddrLedStrip_t;

typedef struct {
  AddrLedStrip_t *strip;
  Pixel_t *stripFirstPixel;
  uint16_t numLeds;
  uint16_t stripRange[2]; // Inclusive
  Position_e position;
  struct AddrLedPanel_t *neighborPanels[4];
} AddrLedPanel_t;

void AddrLedDriver_Init(void);
void AddrLedDriver_DisplayStrip(AddrLedStrip_t *l);
void AddrLedDriver_DisplayCube(void);
bool AddrLedDriver_ShouldRedraw(void);
void AddrLedDriver_StartPollTimer(void);
void AddrLedDriver_StartPollTimer(void);
void AddrLedDriver_SetPixelRgb(Pixel_t *p, uint8_t r, uint8_t g, uint8_t b);
void AddrLedDriver_SetPixelRgbInPanel(Position_e pos, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void AddrLedDriver_Clear(void);
char * AddrLedDriver_GetPositionString(Position_e pos);
AddrLedPanel_t* AddrLedDriver_GetPanelByLocation(Position_e pos);
Pixel_t* AddrLedDriver_GetPixelInPanel(Position_e pos, uint8_t x, uint8_t y);
AddrLedStrip_t* AddrLedDriver_GetStrip(void);
void AddrLedDriver_TakeUsrCommand(uint8_t argc, char **argv);
void AddrLedDriver_PrintPixels(void);
void AddrLedDriver_PrintPixelsRaw(void);
void AddrLedDriver_Test(void);
#endif
