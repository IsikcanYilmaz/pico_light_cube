#include "animation_scroller.h"
#include "addr_led_driver.h"

bool AnimationCanvas_Init(void *arg)
{
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

			break;
		}
		case ANIMATION_SIGNAL_STOP:
		{
			
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
	
}
