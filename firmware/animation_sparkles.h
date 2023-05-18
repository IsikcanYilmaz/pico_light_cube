#include "button.h"

bool AnimationSparkles_Init(void *arg);
void AnimationSparkles_Deinit(void);
void AnimationSparkles_Start(void);
void AnimationSparkles_Stop(void);
void AnimationSparkles_Update(void);
void AnimationSparkles_Draw(void);
void AnimationSparkles_ButtonInput(Button_e b, ButtonGesture_e g);
void AnimationSparkles_UsrInput(uint8_t argc, char **argv);
