#include "button.h"
#include <stdbool.h>

bool AnimationScroller_Init(void *arg);
void AnimationScroller_Deinit(void);
void AnimationScroller_Start(void);
void AnimationScroller_Stop(void);
void AnimationScroller_Update(void);
void AnimationScroller_Draw(void);
void AnimationScroller_ButtonInput(Button_e b, ButtonGesture_e g);
void AnimationScroller_UsrInput(uint8_t argc, char **argv);
