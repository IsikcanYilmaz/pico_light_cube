#include "animation_scroller.h"
#include "addr_led_driver.h"
#include "color.h"
#include <math.h>
#include <stdio.h>

bool AnimationScroller_Init(void *arg)
{
	// set pixel init colors of all panels except top
	for (uint8_t pos = 0; pos < TOP; pos++)
	{
		for (uint8_t row = 0; row < NUM_LEDS_PER_PANEL_SIDE; row++)
		{
			for (uint8_t col = 0; col < NUM_LEDS_PER_PANEL_SIDE; col++)
			{
				Pixel_t *p = AddrLedDriver_GetPixelInPanel(pos, col, row);
				Color_t c = Color_CreateFromHsv(row * 10, 0.80, 0.80);
				// printf("p %x, %s %d %d, h %f\n", p, AddrLedDriver_GetPositionString(pos), col, row, c.hue);
				AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
			}
		}
	}

	// do top seperately	
	for (uint8_t row = 0; row < NUM_LEDS_PER_PANEL_SIDE; row++)
	{
		for (uint8_t col = 0; col < NUM_LEDS_PER_PANEL_SIDE; col++)
		{
			Pixel_t *p = AddrLedDriver_GetPixelInPanel(TOP, col, row);
			bool midSection = ((row == 1 && col == 1) || (row == 2	&& col == 1) || (row == 1 && col == 2) || (row == 2 && col == 2));
			Color_t c = Color_CreateFromHsv(midSection ? 5 * 10 : 4 * 10 , 0.80, 0.80);
			AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
		}
	}
}

void AnimationScroller_Deinit(void)
{
}

void AnimationScroller_Start(void)
{
}

void AnimationScroller_Stop(void)
{
}

void AnimationScroller_Update(void)
{
	// set pixel init colors of all panels except top
	for (uint8_t pos = 0; pos < TOP; pos++)
	{
		for (uint8_t row = 0; row < NUM_LEDS_PER_PANEL_SIDE; row++)
		{
			for (uint8_t col = 0; col < NUM_LEDS_PER_PANEL_SIDE; col++)
			{
				Pixel_t *p = AddrLedDriver_GetPixelInPanel(pos, col, row);
				Color_t c = Color_CreateFromRgb(p->red, p->green, p->blue);
				c = Color_CreateFromHsv(c.hue+1, c.saturation, c.value);
				AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
			}
		}
	}

	// do top seperately	
	for (uint8_t row = 0; row < NUM_LEDS_PER_PANEL_SIDE; row++)
	{
		for (uint8_t col = 0; col < NUM_LEDS_PER_PANEL_SIDE; col++)
		{
			Pixel_t *p = AddrLedDriver_GetPixelInPanel(TOP, col, row);
			Color_t c = Color_CreateFromRgb(p->red, p->green, p->blue);
			c = Color_CreateFromHsv(c.hue+1, c.saturation, c.value);
			AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
		}
	}
}

void AnimationScroller_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationScroller_UsrInput(uint8_t argc, char **argv)
{
}
