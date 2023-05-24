#include "button.h"
#include "animation_manager.h"

typedef enum 
{
	SPARKLES_MODE_RANDOM,
	SPARKLES_MODE_FIXED, // TODO
	SPARKLES_MODE_MAX
} SparkesMode_e;

bool AnimationSparkles_Init(void *arg);
void AnimationSparkles_Deinit(void);
void AnimationSparkles_Start(void);
void AnimationSparkles_Stop(void);
void AnimationSparkles_Update(void);
void AnimationSparkles_Draw(void);
void AnimationSparkles_ButtonInput(Button_e b, ButtonGesture_e g);
void AnimationSparkles_UsrInput(uint8_t argc, char **argv);
void AnimationSparkles_ReceiveSignal(AnimationSignal_e s);
AnimationState_e AnimationSparkles_GetState(void);
