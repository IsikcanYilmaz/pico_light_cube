#include "animation_manager.h"
#include "animation_scroller.h"
#include "animation_canvas.h"
#include "addr_led_driver.h"
#include "animation_sparkles.h"
#include "pico/time.h"
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"

repeating_timer_t animationManUpdateTimer;
Animation_s *currentAnimation;

Animation_s animations[ANIMATION_MAX] = {
	[ANIMATION_SCROLLER] = {
		.name = "scroller",
		.init = AnimationScroller_Init,
		.deinit = AnimationScroller_Deinit,
		.start = AnimationScroller_Start,
		.stop = AnimationScroller_Stop,
		.update = AnimationScroller_Update,
		.buttonInput = AnimationScroller_ButtonInput,
		.usrInput = AnimationScroller_UsrInput,
	},
	[ANIMATION_SPARKLES] = {
		.name = "sparkles",
		.init = AnimationSparkles_Init,
		.deinit = AnimationSparkles_Deinit,
		.start = AnimationSparkles_Start,
		.stop = AnimationSparkles_Stop,
		.update = AnimationSparkles_Update,
		.buttonInput = AnimationSparkles_ButtonInput,
		.usrInput = AnimationSparkles_UsrInput
	},
	[ANIMATION_CANVAS] = {
		.name = "canvas",
		.init = AnimationCanvas_Init,
		.deinit = AnimationCanvas_Deinit,
		.start = AnimationCanvas_Start,
		.stop = AnimationCanvas_Stop,
		.update = AnimationCanvas_Update,
		.buttonInput = AnimationCanvas_ButtonInput,
		.usrInput = AnimationCanvas_UsrInput,
	},
};

bool AnimationMan_PollCallback(repeating_timer_t *t)
{
	currentAnimation->update();
	if (AddrLedDriver_ShouldRedraw()) // TODO why didnt this work?
	{
		AddrLedDriver_DisplayCube();
	}
	return true;
}

static Animation_s * AnimationMan_GetAnimationByIdx(AnimationIdx_e idx)
{
	if (idx >= ANIMATION_MAX)
	{
		printf("Bad anim idx %d at %s\n", idx, __FUNCTION__);
		return NULL;
	}
	return &animations[idx];
}

void AnimationMan_Init(void)
{
	currentAnimation = AnimationMan_GetAnimationByIdx(ANIMATION_DEFAULT);
	currentAnimation->init(NULL);
	AnimationMan_StartPollTimer();
}

void AnimationMan_StartPollTimer(void)
{
	add_repeating_timer_ms(ANIMATION_UPDATE_PERIOD_MS, AnimationMan_PollCallback, NULL, &(animationManUpdateTimer));
}

void AnimationMan_StopPollTimer(void)
{
	cancel_repeating_timer(&(animationManUpdateTimer));
}

void AnimationMan_SetAnimation(AnimationIdx_e anim, bool immediately)
{
	AnimationMan_StopPollTimer();
	if (anim >= ANIMATION_MAX)
	{
		printf("Bad anim idx %d to %s\n", anim, __FUNCTION__);
		return;
	}

	// TODO make this so that this sends a signal to the running animation which then does its cleanup and fades off
	// for now, it abruptly changes
	currentAnimation->deinit();
	AddrLedDriver_Clear();
	currentAnimation = &animations[anim];
	currentAnimation->init(NULL);
	AnimationMan_StartPollTimer();
}

void AnimationMan_TakeUsrCommand(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	if (strcmp(argv[1], "set") == 0)
	{
		ASSERT_ARGS(3);
		for (int i = 0; i < ANIMATION_MAX; i++)
		{
			if (strcmp(argv[2], animations[i].name) == 0)
			{
				AnimationMan_SetAnimation(i, false);
				return;
			}
		}
	}
	else
	{
		currentAnimation->usrInput(argc, argv);
	}
}
