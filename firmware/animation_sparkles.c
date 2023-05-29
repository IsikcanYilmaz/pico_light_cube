#include "animation_sparkles.h"
#include "addr_led_driver.h"
#include "color.h"
#include "pico/rand.h"
#include "visual_util.h"
#include "editable_value.h"
#include "usr_commands.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_H 0.00
#define DEFAULT_S 0.80
#define DEFAULT_V 0.80

#define DEFAULT_RANDOM_UPPER_LIM_H 360.0 // 345.0
#define DEFAULT_RANDOM_UPPER_LIM_S 1.0
#define DEFAULT_RANDOM_UPPER_LIM_V 1.0

#define DEFAULT_RANDOM_LOWER_LIM_H 0.0 // 250.0
#define DEFAULT_RANDOM_LOWER_LIM_S 0.8
#define DEFAULT_RANDOM_LOWER_LIM_V 0.8

#define DEFAULT_SPARKLE_CHANCE_PERCENT 100

#define DEFAULT_ITER_UNTIL_HUE_CHANGE 300

#define DEFAULT_PIXEL_H_CHANGE_PER_ITER 0.0
#define DEFAULT_PIXEL_S_CHANGE_PER_ITER 0.0
#define DEFAULT_PIXEL_V_CHANGE_PER_ITER -0.01

#define DEFAULT_NUM_COLORS 2
#define MAX_COLORS 10

// 
static uint16_t iter = 0;

static uint8_t numColors = DEFAULT_NUM_COLORS; // editable
static Color_t colorArr[MAX_COLORS];
static Color_t *currColor = &colorArr[0];
static uint8_t colorIdx = 0;

static SparklesSparkMode_e sparkMode = SPARKLES_MODE_DROPS;
static SparklesColorMode_e colorMode = SPARKLES_COLOR_RANDOM;

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

static uint8_t sparkleChancePercent = DEFAULT_SPARKLE_CHANCE_PERCENT;

static volatile AnimationState_e state = ANIMATION_STATE_UNINITIALIZED;

static double testval = 0.3;

static EditableValue_t editableValues[] = {
	(EditableValue_t) {.name = "numColors", .valPtr = (uint8_t *) &numColors, .type = UINT8_T, .lowerLimit = 0, .upperLimit = MAX_COLORS},
	(EditableValue_t) {.name = "sparkleChancePercent", .valPtr = (uint8_t *) &sparkleChancePercent, .type = UINT8_T, .lowerLimit = 0, .upperLimit = 100},
	(EditableValue_t) {.name = "randomLowerLimH", .valPtr = (uint8_t *) &randomLowerLimH, .type = DOUBLE, .lowerLimit = 0.00, .upperLimit = 360.00},
	(EditableValue_t) {.name = "randomUpperLimH", .valPtr = (uint8_t *) &randomUpperLimH, .type = DOUBLE, .lowerLimit = 0.00, .upperLimit = 360.00},
	(EditableValue_t) {.name = "randomLowerLimS", .valPtr = (uint8_t *) &randomLowerLimS, .type = DOUBLE, .lowerLimit = 0.00, .upperLimit = 1.00},
	(EditableValue_t) {.name = "randomUpperLimS", .valPtr = (uint8_t *) &randomUpperLimS, .type = DOUBLE, .lowerLimit = 0.00, .upperLimit = 1.00},
	(EditableValue_t) {.name = "randomLowerLimV", .valPtr = (uint8_t *) &randomLowerLimV, .type = DOUBLE, .lowerLimit = 0.00, .upperLimit = 1.00},
	(EditableValue_t) {.name = "randomUpperLimV", .valPtr = (uint8_t *) &randomUpperLimV, .type = DOUBLE, .lowerLimit = 0.00, .upperLimit = 1.00},
	(EditableValue_t) {.name = "hChange", .valPtr = (uint8_t *) &hChange, .type = DOUBLE, .lowerLimit = (double) -0.01, .upperLimit = 360.00},
	(EditableValue_t) {.name = "sChange", .valPtr = (uint8_t *) &sChange, .type = DOUBLE, .lowerLimit = (double) -0.01, .upperLimit = 1.00},
	(EditableValue_t) {.name = "vChange", .valPtr = (uint8_t *) &vChange, .type = DOUBLE, .lowerLimit = (double) -0.01, .upperLimit = 1.00},
	(EditableValue_t) {.name = "iterUntilChange", .valPtr = (uint8_t *) &iterUntilChange, .type = UINT16_T, .lowerLimit = (uint16_t) 10, .upperLimit = (uint16_t) 0xffff},
	(EditableValue_t) {.name = "testval", .valPtr = (uint8_t *) &testval, .type = DOUBLE, .lowerLimit = (double) -0.1, .upperLimit = (double) 1.0},
};
static EditableValueList_t editableValuesList = {.values = &editableValues[0], .len = sizeof(editableValues)/sizeof(EditableValue_t)};

uint8_t numVacancies = NUM_LEDS;
uint8_t vacancyIndexes[NUM_LEDS];

static Color_t GenerateRandomColor(void)
{
	double h = remainder(get_rand_32(), (randomUpperLimH - randomLowerLimH + 1)) + randomLowerLimH;
	double s = (double) randomLowerLimS + remainder((get_rand_32() % 1000) / 100, (randomUpperLimS - randomLowerLimS + 0.01));
	double v = (double) randomLowerLimV + remainder((get_rand_32() % 1000) / 100, (randomUpperLimV - randomLowerLimV + 0.01));
	Color_t c = Color_CreateFromHsv(h, s, v);
	// Color_PrintColor(c);
	return c;
}

static void SetCurrColorRandomly(void)
{
	if (numColors > 1)
	{
		currColor = &colorArr[get_rand_32() % numColors];
	}
}

static void CheckVacancies(void)
{
	AddrLedStrip_t *s = AddrLedDriver_GetStrip();
	numVacancies = 0;
	for (uint8_t i = 0; i < s->numLeds; i++)
	{
		Pixel_t *p = &(s->pixels[i]); 
		if (Visual_IsDark(p))
		{
			vacancyIndexes[numVacancies] = i;
			numVacancies++;
		}
	}
}

static void RunningAction(void)
{
	CheckVacancies();
	if (iter >= iterUntilChange)
	{
		*currColor = GenerateRandomColor();
		colorIdx = colorIdx % numColors;
		iter = 0;
	}
	SetCurrColorRandomly();
	if (get_rand_32() % 100 < sparkleChancePercent)
	{
		bool emptyCell = false;
		Pixel_t *p;
		AddrLedStrip_t *s = AddrLedDriver_GetStrip();
		uint8_t randTableIdx = (uint8_t) get_rand_32() % numVacancies;
		uint8_t randLedIdx = vacancyIndexes[randTableIdx];
		p = &(s->pixels[randLedIdx]); 
		if (Visual_IsDark(p))
		{
			AddrLedDriver_SetPixelRgb(p, currColor->red, currColor->green, currColor->blue);
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

static void InitColors(void)
{
	for (uint8_t i = 0; i < MAX_COLORS; i++)
	{
		colorArr[i] = GenerateRandomColor();
	}
}

static void IterThruColors(void)
{
	colorIdx = (colorIdx + 1) % numColors;
	colorArr[colorIdx] = GenerateRandomColor();
}

bool AnimationSparkles_Init(void *arg)
{
	InitColors();
	AddrLedDriver_Clear();
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
	ASSERT_ARGS(1);
	printf("Sparkles received usr input:");
	for (int i = 0; i < argc; i++)
	{
		printf(" %s", argv[i]);
	}
	printf("\n");

	if (strcmp(argv[0], "setval") == 0)
	{
		ASSERT_ARGS(3);
		bool ret = EditableValue_FindAndSetValueFromString(&editableValuesList, argv[1], argv[2]);
		printf("%s set to %s %s\n", argv[1], argv[2], (ret) ? "SUCCESS" : "FAIL");
	}
	else if (strcmp(argv[0], "getval") == 0)
	{
		EditableValue_PrintList(&editableValuesList);
	}
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

