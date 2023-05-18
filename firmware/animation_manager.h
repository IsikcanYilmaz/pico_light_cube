#include <stdbool.h>
#include "button.h"

#define ANIMATION_UPDATE_PERIOD_MS 10
#define ANIMATION_DEFAULT (ANIMATION_SPARKLES) //(ANIMATION_CANVAS)

typedef enum AnimationIdx_e_
{
	ANIMATION_SCROLLER,
	ANIMATION_CANVAS,
	ANIMATION_SPARKLES,
	ANIMATION_MAX
} AnimationIdx_e;

typedef enum AnimationManState_e_
{
	ANIMATION_STATE_PLAY,
	ANIMATION_STATE_PAUSE,
	ANIMATION_STATE_SWITCHING,
	ANIMATION_STATE_MAX
} AnimationManState_e;

typedef struct Animation_s_
{
	char *name;
	bool (*init)(void *arg);
	void (*deinit)(void);
	void (*start)(void);
	void (*stop)(void);
	void (*update)(void);
	void (*buttonInput)(Button_e b, ButtonGesture_e g);
	void (*usrInput)(uint8_t argc, char **argv);
} Animation_s;

void AnimationMan_Init(void);
void AnimationMan_StartPollTimer(void);
void AnimationMan_StopPollTimer(void);
void AnimationMan_SetAnimation(AnimationIdx_e anim, bool immediately);
void AnimationMan_TakeUsrCommand(uint8_t argc, char **argv);
