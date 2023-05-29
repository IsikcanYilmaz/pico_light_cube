#include "animation_scroller.h"
#include "addr_led_driver.h"
#include "color.h"
#include "visual_util.h"
#include <math.h>
#include <stdio.h>

#define DEFAULT_H 0.80
#define DEFAULT_S 0.80
#define DEFAULT_V 0.00

#define DEFAULT_UL_H 360.0
#define DEFAULT_UL_S 0.80
#define DEFAULT_UL_V 0.50

#define DEFAULT_LL_H 0.0
#define DEFAULT_LL_S 0.50
#define DEFAULT_LL_V 0.40

static AnimationState_e state = ANIMATION_STATE_UNINITIALIZED;

static double currH = DEFAULT_H;
static double currS = DEFAULT_S;
static double currV = DEFAULT_V;

static double ulH = DEFAULT_UL_H;
static double ulS = DEFAULT_UL_S;
static double ulV = DEFAULT_UL_V;

static double llH = DEFAULT_LL_H;
static double llS = DEFAULT_LL_S;
static double llV = DEFAULT_LL_V;

static double rowHDiff = 20;
static double incrementH = 0.5;
static double phaseH = 0.0;

static void RunningAction(void)
{
	phaseH += incrementH;
	phaseH = remainder(phaseH, ulH);

	// set pixel init colors of all panels except top
	for (uint8_t pos = 0; pos < TOP; pos++)
	{
		for (uint8_t row = 0; row < NUM_LEDS_PER_PANEL_SIDE; row++)
		{
			for (uint8_t col = 0; col < NUM_LEDS_PER_PANEL_SIDE; col++)
			{
				Pixel_t *p = AddrLedDriver_GetPixelInPanel(pos, col, row);
				Color_t c = Color_CreateFromHsv(phaseH + row * rowHDiff, currS, currV);
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
			Color_t c = Color_CreateFromHsv(phaseH + (midSection ? 5 * 10 : 4 * 10) , currS, currV);
			AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
		}
	}
}

static void FadeOffAction(void)
{
	// If we're stopping, fade off all LEDs. Check everytime if all LEDs are off
	currV -= 0.01;
	if (Visual_IsAllDark())
	{
		state = ANIMATION_STATE_STOPPED;
		printf("Fade off done state %d\n", state);
	}
}

static void FadeUpAction(void)
{
	// Visual_IncrementAllByHSV(0,0,0.01);
	// // Check on any pixel if we've received our V value
	// Pixel_t *p = &(AddrLedDriver_GetStrip()->pixels[0]);
	// Color_t c = Color_CreateFromRgb(p->red, p->green, p->blue);
	currV += 0.01;
	if (currV >= ulV)
	{
		state = ANIMATION_STATE_RUNNING;
		printf("Fade up done state %d\n", state);
	}
}

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
				Color_t c = Color_CreateFromHsv(row * 10, DEFAULT_S, DEFAULT_V);
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
			Color_t c = Color_CreateFromHsv(midSection ? 5 * 10 : 4 * 10 , DEFAULT_S, DEFAULT_V);
			AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
		}
	}

	state = ANIMATION_STATE_STARTING;
	printf("%s\n", __FUNCTION__);
}

void AnimationScroller_Deinit(void)
{
	// TODO when/if i end up using a dynamic allocator i'll do freeing here
	state = ANIMATION_STATE_UNINITIALIZED;
}

void AnimationScroller_Start(void)
{
}

void AnimationScroller_Stop(void)
{
}

void AnimationScroller_Update(void)
{
	switch(state)
	{
		case ANIMATION_STATE_STARTING:
		{
			RunningAction();
			FadeUpAction();
			break;
		}
		case ANIMATION_STATE_RUNNING:
		{
			RunningAction();
			break;
		}
		case ANIMATION_STATE_STOPPING:
		{
			RunningAction();
			FadeOffAction();
			break;
		}
		case ANIMATION_STATE_STOPPED:
		{
			// NOP
			break;
		}
		default:
		{
			break;
		}
	}


}

void AnimationScroller_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationScroller_UsrInput(uint8_t argc, char **argv)
{
}

void AnimationScroller_ReceiveSignal(AnimationSignal_e s)
{
	switch(s)
	{
		case ANIMATION_SIGNAL_START:
		{
			state = ANIMATION_STATE_STARTING;
			break;
		}
		case ANIMATION_SIGNAL_STOP:
		{
			state = ANIMATION_STATE_STOPPING;
			break;
		}
		default:
		{

			break;
		}
	}
}

AnimationState_e AnimationScroller_GetState(void)
{
	return state;
}
