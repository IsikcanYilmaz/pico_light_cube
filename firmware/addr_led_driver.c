#include "addr_led_driver.h"
#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/rand.h"
#include "usr_commands.h"

uint pwm_chan, slice_num;
int dma_chan;

Pixel_t ledStrip0Pixels[NUM_LEDS];
AddrLedStrip_t ledStrip0;
AddrLedPanel_t ledPanels[NUM_PANELS];
PixelPacketBuffer_t ledStrip0PacketBuffer[SIGNAL_BUFFER_LEN];
repeating_timer_t ledUpdateTimer;

bool pixelChanged = true;

char *positionStrings[NUM_SIDES] = {
	[NORTH] = "NORTH",
	[EAST] = "EAST",
	[SOUTH] = "SOUTH",
	[WEST] = "WEST",
	[TOP] = "TOP"
};

// WS2812B Related 

// Convert $byte into codes that WS2812B accepts, that we push out via PWM.
// 1 Byte converts into 8 bytes, so make sure $*codes pointer points to a block of memory that has 8 bytes allocated
static inline void ByteToCodes(uint8_t byte, uint16_t *codes)
{
  for (int b = 0; b < 8; b++)
  {
    codes[7-b] = ((byte & (0x1 << b)) > 0) ? ADDR_LED_PWM_HI_CC_VAL : ADDR_LED_PWM_LO_CC_VAL;
  }
}

// Convert a Pixel_t object $p into codes that WS2812B accepts. 
// 1 Byte converts into 8 bytes, a Pixel_t is 3 bytes so make sure $*packet points to a block of memory that has 24 bytes allocated
static inline void PixelToPacket(Pixel_t *pixel, PixelPacket_t *packet)
{
  ByteToCodes(pixel->green, &(packet->greenRaw[0]));
  ByteToCodes(pixel->red, &(packet->redRaw[0]));
  ByteToCodes(pixel->blue, &(packet->blueRaw[0]));
}


static void InitPanel(AddrLedPanel_t *p)
{
  // Set local coordinates of this panel
  Pixel_t *pixels = p->strip->pixels;
  for (int i = 0; i < p->numLeds; i++)
  {
    Pixel_t *pixel = &pixels[p->stripRange[0] + i];
    pixel->x = NUM_LEDS_PER_PANEL_SIDE - (i / NUM_LEDS_PER_PANEL_SIDE);
    pixel->y = NUM_LEDS_PER_PANEL_SIDE - (i % NUM_LEDS_PER_PANEL_SIDE);
    
    // TODO // HANDLE GLOBAL COORDINATES
  }

  // Set global coordinates of this panel

}

static bool AddrLedDriver_UpdateCallback(struct repeating_timer_t *t)
{
	if (pixelChanged)
	{
		AddrLedDriver_DisplayStrip(&ledStrip0);
		printf("Displayed strip\n");
	}
}

void AddrLedDriver_Init(void)
{
	// First init our data structures 
	// Initialize the strip(s). This initializes one continuous strip. 
  // If multiple panels are daisychained, that counts as one strip.
  ledStrip0 = (AddrLedStrip_t) {
    .numLeds                 = NUM_LEDS,
    .pixels                  = (Pixel_t *) &ledStrip0Pixels,
    .pixelPacketBuffer       = (PixelPacketBuffer_t *) &ledStrip0PacketBuffer,
  };
  memset(&ledStrip0PacketBuffer, 0x0, SIGNAL_BUFFER_LEN * sizeof(PixelPacketBuffer_t));

  // Initialize the panel structures
  for (int panelIdx = 0; panelIdx < NUM_PANELS; panelIdx++)
  {
    Position_e pos = (Position_e) panelIdx;
    AddrLedPanel_t p = {
      .strip = &ledStrip0,
      .numLeds = NUM_LEDS_PER_PANEL,
      .stripRange = {(panelIdx * NUM_LEDS_PER_PANEL), ((panelIdx + 1) * NUM_LEDS_PER_PANEL - 1)},
      .position = pos,
      .neighborPanels = {NULL, NULL, NULL, NULL},
    };
    p.stripFirstPixel = &(p.strip->pixels[p.stripRange[0]]);
    InitPanel(&p);
    ledPanels[pos] = p;
  }

	// Then init our hardware
	pwm_chan = pwm_gpio_to_channel(ADDR_LED_SIGNAL_GPIO_PIN);
	gpio_set_function(ADDR_LED_SIGNAL_GPIO_PIN, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(ADDR_LED_SIGNAL_GPIO_PIN);
	float divider = ADDR_LED_PWM_DIVIDER;
	pwm_set_clkdiv(slice_num, divider);
	pwm_set_wrap(slice_num, ADDR_LED_PWM_WRAP);
	pwm_set_enabled(slice_num, true);
	pwm_set_irq_enabled(slice_num, true); // Necessary? Yes 
	dma_chan = dma_claim_unused_channel(true);
	dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
	channel_config_set_read_increment(&cfg, true);
	channel_config_set_dreq(&cfg, DREQ_PWM_WRAP0 + slice_num); // ?
	dma_channel_configure(
		dma_chan,                     // channel to be configured
		&cfg,                         // conf we just created
		&pwm_hw->slice[slice_num].cc, // initial write address
		ledStrip0PacketBuffer,        // initial read address
		SIGNAL_BUFFER_LEN,            // number of transfers
		false                         // start immediately
	);

	// start our update timer
	// AddrLedDriver_StartPollTimer();
}

// Convert our pixel data to raw signal codes and push em out via the dma/pwm
void AddrLedDriver_DisplayStrip(AddrLedStrip_t *l)
{
	PixelPacket_t *pixelPacketBufferPtr = (PixelPacket_t *) (l->pixelPacketBuffer);
  for (int i = 0; i < l->numLeds; i++)
  {
    Pixel_t *currPixel = &(l->pixels[i]);
    PixelPacket_t *currPixelPacket = pixelPacketBufferPtr + i;
    PixelToPacket(currPixel, currPixelPacket);
  }
	dma_channel_transfer_from_buffer_now(dma_chan, pixelPacketBufferPtr, sizeof(PixelPacket_t) * NUM_LEDS);
	dma_channel_wait_for_finish_blocking(dma_chan);
	pwm_set_gpio_level(ADDR_LED_SIGNAL_GPIO_PIN, 0);
	pixelChanged = false;
}

void AddrLedDriver_StartPollTimer(void)
{
	add_repeating_timer_ms(ADDR_LED_UPDATE_PERIOD_MS, AddrLedDriver_UpdateCallback, NULL, &(ledUpdateTimer));
}

void OledMan_StopPollTimer(void)
{
	cancel_repeating_timer(&(ledUpdateTimer));
}

void AddrLedDriver_SetPixelRgb(Pixel_t *p, uint8_t r, uint8_t g, uint8_t b)
{
  p->red = r;
  p->green = g;
  p->blue = b;
	pixelChanged = true;
}

// ! Below depends on the LED structs that this file initializes. these could reside in manager code? but not now
// fuck it. this _is_ the manager code. until further notice.
void AddrLedDriver_SetPixelRgbInPanel(Position_e pos, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
  // sanity checks
  if (pos >= NUM_SIDES || x > NUM_LEDS_PER_PANEL_SIDE || y > NUM_LEDS_PER_PANEL_SIDE)
  {
    printf("Incorrect args to SetPixelRgb %d %d %d\n", pos, x, y);
    return;  // TODO have an error type
  }
  AddrLedPanel_t *panel = AddrLedDriver_GetPanelByLocation(pos);
  Pixel_t *pixel = AddrLedDriver_GetPixelInPanel(pos, x, y);
	AddrLedDriver_SetPixelRgb(pixel, r, g, b);
}

Pixel_t* AddrLedDriver_GetPixelInPanel(Position_e pos, uint8_t x, uint8_t y)
{
  AddrLedPanel_t *panel = AddrLedDriver_GetPanelByLocation(pos);
  AddrLedStrip_t *strip = panel->strip;
  uint8_t ledIdx;
#if LEDS_BEGIN_AT_BOTTOM
  y = NUM_LEDS_PER_PANEL_SIDE - y - 1;
#endif
	y = NUM_LEDS_PER_PANEL_SIDE - 1 - y;// HACK ! y seems inverted. flip it
	
  if (y % 2 == 0)
  {
    ledIdx = x + (NUM_LEDS_PER_PANEL_SIDE * y);
  }
  else
  {
    ledIdx = (NUM_LEDS_PER_PANEL_SIDE - 1 - x) + (NUM_LEDS_PER_PANEL_SIDE * y);
  }
  return &(panel->stripFirstPixel[ledIdx]);
}

AddrLedPanel_t* AddrLedDriver_GetPanelByLocation(Position_e pos)
{
	if (pos >= NUM_SIDES)
	{
		printf("Bad pos %d for %s\n", pos, __FUNCTION__);
		return NULL;
	}
  return &ledPanels[pos];
}

AddrLedStrip_t* AddrLedDriver_GetStrip(void)
{
	return &ledStrip0;
}

// This is so that we dont expose $ledstrip0 outside
void AddrLedDriver_DisplayCube(void)
{
	AddrLedDriver_DisplayStrip(&ledStrip0);
}

void AddrLedDriver_Clear(void)
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		AddrLedDriver_SetPixelRgb(&ledStrip0.pixels[i], 0, 0, 0);
	}
}

char * AddrLedDriver_GetPositionString(Position_e pos)
{
	if (pos >= NUM_SIDES)
	{
		printf("Bad pos %d to %s\n", pos, __FUNCTION__);
		return NULL;
	}
	return positionStrings[pos];
}

bool AddrLedDriver_ShouldRedraw(void)
{
	return pixelChanged;
}

void AddrLedDriver_PrintPixelsRaw(void)
{
	// TODO print pixel values in byte from for cmd line tools
}

void AddrLedDriver_TakeUsrCommand(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	if (strcmp(argv[1], "clear") == 0)
	{
		AddrLedDriver_Clear();
	}
	else if (strcmp(argv[1], "set") == 0)
	{
		// aled set <pos> <x> <y> <r> <g> <b>
		ASSERT_ARGS(8);
		Position_e pos = NUM_SIDES;
		if (strcmp(argv[2], "n") == 0)
		{
			pos = NORTH;
		}
		else if (strcmp(argv[2], "e") == 0)
		{
			pos = EAST;
		}
		else if (strcmp(argv[2], "s") == 0)
		{
			pos = SOUTH;
		}
		else if (strcmp(argv[2], "w") == 0)
		{
			pos = WEST;
		}
		else if (strcmp(argv[2], "t") == 0)
		{
			pos = TOP;
		}
		else
		{
			printf("BAD SIDE DESCRIPTOR! %s\n", argv[0]);
			return;
		}
		uint8_t x = atoi(argv[3]);
		uint8_t y = atoi(argv[4]);
		uint8_t r = atoi(argv[5]);
		uint8_t g = atoi(argv[6]);
		uint8_t b = atoi(argv[7]);
		printf("Setting pixel %s %d %d to %d %d %d\n", AddrLedDriver_GetPositionString(pos), x, y, r, g, b);
		AddrLedDriver_SetPixelRgbInPanel(pos, x, y, r, g, b);
	}
}

void AddrLedDriver_Test(void)
{
 	for (int pos = 0; pos < NUM_SIDES; pos++)
	{
		for (int x = 0; x < NUM_LEDS_PER_PANEL_SIDE; x++)
		{
			for (int y = 0; y < NUM_LEDS_PER_PANEL_SIDE; y++)
			{
				Pixel_t *p = AddrLedDriver_GetPixelInPanel(pos, x, y);
				uint8_t color[3] = {0,0,0};
				switch(pos)
				{
					case NORTH:
					{
						color[0] = (y + 1) * 60;
						break;
					}
					case EAST:
					{
						color[1] = (y + 1) * 60;
						break;
					}
					case SOUTH:
					{
						color[2] = (y + 1) * 60;
						break;
					}
					case WEST:
					{
						color[0] = (y + 1) * 60;
						color[1] = (y + 1) * 60;
						break;
					}
					case TOP:
					{
						color[0] = (y + 1) * 60;
						color[1] = (y + 1) * 60;
						color[2] = (y + 1) * 60;
						break;
					}
					default:
					{
					}
				}
				AddrLedDriver_SetPixelRgb(p, color[0], color[1], color[2]);
			}
		}
	}
	AddrLedDriver_DisplayStrip(&ledStrip0);
}

void AddrLedDriver_PrintPixels(void)
{
	for (int pos = 0; pos < NUM_SIDES; pos++)
	{
		for (int row = 0; row < NUM_LEDS_PER_PANEL; row++)
		{
			for (int col = 0; col < NUM_LEDS_PER_PANEL; col++)
			{
				Pixel_t *p = AddrLedDriver_GetPixelInPanel(pos, col, row);
				printf("p %s %d %d | r %d g %d b %d\n", AddrLedDriver_GetPositionString(pos), col, row, p->red, p->green, p->blue);
			}
		}
	}
}

