#include "animation_sparkles.h"
#include "addr_led_driver.h"
#include "color.h"
#include "pico/rand.h"
#include "visual_util.h"
#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_H 0.00
#define DEFAULT_S 0.80
#define DEFAULT_V 0.80

#define DEFAULT_RANDOM_UPPER_LIM_H 360.0 // 345.0
#define DEFAULT_RANDOM_UPPER_LIM_S 1.0
#define DEFAULT_RANDOM_UPPER_LIM_V 1.0

#define DEFAULT_RANDOM_LOWER_LIM_H 0.0 // 250.0
#define DEFAULT_RANDOM_LOWER_LIM_S 0.50
#define DEFAULT_RANDOM_LOWER_LIM_V 0.50

#define DEFAULT_SPARKLE_CHANCE_PERCENT 100

#define DEFAULT_ITER_UNTIL_HUE_CHANGE 100

#define DEFAULT_PIXEL_H_CHANGE_PER_ITER 0.0
#define DEFAULT_PIXEL_S_CHANGE_PER_ITER 0.0
#define DEFAULT_PIXEL_V_CHANGE_PER_ITER -0.01

// 
static uint16_t iter = 0;
static Color_t currColor;

static double randomLowerLimH = DEFAULT_RANDOM_LOWER_LIM_H;
static double randomUpperLimH = DEFAULT_RANDOM_UPPER_LIM_H;

static double randomLowerLimS = DEFAULT_RANDOM_LOWER_LIM_S;
static double randomUpperLimS = DEFAULT_RANDOM_UPPER_LIM_S;

static double randomLowerLimV = DEFAULT_RANDOM_LOWER_LIM_V;
static double randomUpperLimV = DEFAULT_RANDOM_UPPER_LIM_V;

static uint16_t iterUntilChange = DEFAULT_ITER_UNTIL_HUE_CHANGE;

static double hChange = DEFAULT_PIXEL_H_CHANGE_PER_ITER;
static double sChange = DEFAULT_PIXEL_S_CHANGE_PER_ITER;
static double vChange = DEFAULT_PIXEL_V_CHANGE_PER_ITER;

static uint8_t sparkleChangePercent = DEFAULT_SPARKLE_CHANCE_PERCENT;

static volatile AnimationState_e state = ANIMATION_STATE_UNINITIALIZED;

static Color_t GenerateRandomColor(void)
{
	double h = remainder(get_rand_32(), (randomUpperLimH - randomLowerLimH + 1)) + randomLowerLimH;
	double s = (double) randomLowerLimS + remainder((get_rand_32() % 1000) / 100, (randomUpperLimS - randomLowerLimS + 0.01));
	double v = (double) randomLowerLimV + remainder((get_rand_32() % 1000) / 100, (randomUpperLimV - randomLowerLimV + 0.01));
	Color_t c = Color_CreateFromHsv(h, s, v);
	// Color_PrintColor(c);
	return c;
}

static void RunningAction(void)
{
	if (iter == iterUntilChange)
	{
		currColor = GenerateRandomColor();
		iter = 0;
	}
	if (get_rand_32() % 100 < sparkleChangePercent)
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
		{
			AddrLedDriver_SetPixelRgb(p, currColor.red, currColor.green, currColor.blue);
		}
	}
	Visual_IncrementAllByHSV(hChange,sChange,vChange);
	iter++;
}

static void FadeOffAction(void)
{
	// If we're stopping, fade off all LEDs. Check everytime if all LEDs are off
	Visual_IncrementAllByHSV(0,0,-0.01);
	if (Visual_IsAllDark())
	{
		state = ANIMATION_STATE_STOPPED;
		printf("Fade off done state %d\n", state);
	}
}

bool AnimationSparkles_Init(void *arg)
{
	AddrLedDriver_Clear();
	currColor = GenerateRandomColor();
	state = ANIMATION_STATE_RUNNING;
	printf("%s\n", __FUNCTION__);
}

void AnimationSparkles_Deinit(void)
{
	// TODO when/if i end up using a dynamic allocator i'll do freeing here. for now, STOP basically means UNINITIALIZED
	// state = ANIMATION_STATE_UNINITIALIZED;
}

void AnimationSparkles_Start(void)
{
}

void AnimationSparkles_Stop(void)
{
}

void AnimationSparkles_Update(void)
{
	switch(state)
	{
		case ANIMATION_STATE_STARTING:
		{
			state = ANIMATION_STATE_RUNNING; // TODO populate this area
		}
		case ANIMATION_STATE_RUNNING:
		{
			RunningAction();
			break;
		}
		case ANIMATION_STATE_STOPPING:
		{
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

void AnimationSparkles_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationSparkles_UsrInput(uint8_t argc, char **argv)
{
}

void AnimationSparkles_ReceiveSignal(AnimationSignal_e s)
{
	printf("%s signal received %d\n", __FUNCTION__, s);
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
			printf("%s bad signal %d\n", __FUNCTION__, s);
			break;
		}
	}
}

AnimationState_e AnimationSparkles_GetState(void)
{
	return state;
}

