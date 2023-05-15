#include "animation_scroller.h"
#include "addr_led_driver.h"
#include "color.h"

Color_t c;

bool AnimationScroller_Init(void *arg)
{
	c = Color_CreateFromRgb(0, 0, 0);
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
	static bool goingUp = false;
	static  val = 0;
	if (val == 0 || val == 255)
	{
		goingUp = !goingUp;
	}
	val += (goingUp) ? 1 : -1;
	
	c.blue = 255 - val;
	c.red = val;
}

void AnimationScroller_Draw(void)
{
	AddrLedStrip_t *s = AddrLedDriver_GetStrip();
	for (int i = 0; i < s->numLeds; i++)
	{
		Pixel_t *p = &(s->pixels[i]);
		AddrLedDriver_SetPixelRgb(p, c.red, c.green, c.blue);
	}
}

void AnimationScroller_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationScroller_UsrInput(uint8_t argc, char **argv)
{
}
