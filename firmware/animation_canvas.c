#include "animation_scroller.h"
#include "addr_led_driver.h"
#include "visual_util.h"
#include <stdio.h>

static volatile AnimationState_e state = ANIMATION_STATE_UNINITIALIZED;

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

bool AnimationCanvas_Init(void *arg)
{
	state = ANIMATION_STATE_RUNNING;
}

void AnimationCanvas_Deinit(void)
{
}

void AnimationCanvas_Start(void)
{
}

void AnimationCanvas_Stop(void)
{
}

void AnimationCanvas_Update(void)
{
	switch(state)
	{
		case ANIMATION_STATE_STARTING:
		{
			state = ANIMATION_STATE_RUNNING; // TODO populate this area
		}
		case ANIMATION_STATE_RUNNING:
		{
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

void AnimationCanvas_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationCanvas_UsrInput(uint8_t argc, char **argv)
{
}

void AnimationCanvas_ReceiveSignal(AnimationSignal_e s)
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

AnimationState_e AnimationCanvas_GetState(void)
{
	return state;
}
