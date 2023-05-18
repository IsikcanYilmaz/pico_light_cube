#include "animation_sparkles.h"
#include "addr_led_driver.h"
#include "color.h"
#include "pico/rand.h"
#include <stdbool.h>

#define DEFAULT_S 0.80
#define DEFAULT_V 0.80
#define DEFAULT_H 0.00

#define SPARKLE_CHANCE_PERCENT 20

#define ITER_UNTIL_HUE_CHANGE 300
uint16_t iter = 0;
Color_t currColor;

bool AnimationSparkles_Init(void *arg)
{
	AddrLedDriver_Clear();
	currColor = Color_CreateFromHsv((double) (get_rand_32() % 360), DEFAULT_S, DEFAULT_V); 
}

void AnimationSparkles_Deinit(void)
{
}

void AnimationSparkles_Start(void)
{
}

void AnimationSparkles_Stop(void)
{
}

void AnimationSparkles_Update(void)
{
	if (iter == ITER_UNTIL_HUE_CHANGE)
	{
		currColor = Color_CreateFromHsv((double) (get_rand_32() % 360), DEFAULT_S, DEFAULT_V); 
		iter = 0;
	}
	if (get_rand_32() % 100 < SPARKLE_CHANCE_PERCENT)
	{
		bool emptyCell = false;
		Pixel_t *p;
		Position_e pos = (Position_e) get_rand_32() % NUM_SIDES;
		uint8_t x = (uint8_t) get_rand_32() % NUM_LEDS_PER_PANEL_SIDE;
		uint8_t y = (uint8_t) get_rand_32() % NUM_LEDS_PER_PANEL_SIDE;
		p = AddrLedDriver_GetPixelInPanel(pos, x, y);
		if (!(p->blue || p->green || p->red))
		{
			emptyCell = true;
		}
		if (emptyCell)
			AddrLedDriver_SetPixelRgb(p, currColor.red, currColor.green, currColor.blue);
	}

	for (uint8_t pos = 0; pos < NUM_SIDES; pos++)
	{
		for (uint8_t row = 0; row < NUM_LEDS_PER_PANEL_SIDE; row++)
		{
			for (uint8_t col = 0; col < NUM_LEDS_PER_PANEL_SIDE; col++)
			{
				Pixel_t *p = AddrLedDriver_GetPixelInPanel(pos, col, row);
				Color_t c = Color_CreateFromRgb(p->red, p->green, p->blue);
				// c.saturation = (c.saturation > 0.0) ? c.saturation - 0.01 : c.saturation;
				c.value = (c.value > 0.0) ? c.value - 0.01 : c.value;
				Color_t dc = Color_CreateFromHsv(c.hue, c.saturation, c.value);
				AddrLedDriver_SetPixelRgb(p, dc.red, dc.green, dc.blue);
			}
		}
	}
	iter++;
}

void AnimationSparkles_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationSparkles_UsrInput(uint8_t argc, char **argv)
{
}
