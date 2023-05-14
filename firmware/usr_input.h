#include "pico/util/queue.h"

#define USER_INPUT_BUF_LEN 256
#define USER_INPUT_ENTER_CHAR '\r'
#define USER_INPUT_BKSPACE_CHAR '\b'
#define USER_INPUT_SPACE_CHAR ' '
#define USER_INPUT_PRINT_CURRENT_CHAR_PERIOD_MS 100
#define USER_INPUT_POLL_PERIOD_MS 1

void UserInput_Init(void);
void UserInput_Service(void);
void UserInput_StartPollTimer(void);
void UserInput_StopPollTimer(void);
