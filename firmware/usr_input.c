#include "usr_input.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>
#include "test_functionality.h"
#include "usr_commands.h"

char usrInputBuf[USER_INPUT_BUF_LEN];
char usrCommandBuf[USER_INPUT_BUF_LEN];
char *usrCommandArgs[USER_COMMAND_MAX_ARGS];
volatile uint16_t usrCommandArgc = 0;
volatile uint32_t usrInputIdx = 0;
struct repeating_timer userInputPollTimer;
volatile bool commandReady = false;

static void UserInput_CharAvailCallback(void *args)
{
	// TODO figure this out or delete it
}

static void UserInput_ParseInputLine(char *buf, volatile uint16_t *argc, char **argv, uint16_t len)
{
	strncpy((char *)&usrCommandBuf, buf, len); // TODO inefficient but i just wanna move forward
	char *parseIdx = usrCommandBuf;
	char *arg = parseIdx;
	*argc = 0;
	
	// see if this is a single command or if we have args
	while(true)
	{
		while(*parseIdx != ' ' && *parseIdx != '\0')
		{
			parseIdx++; // TODO point of interest. potential inf loop
		}

		// We skipped the word. parseIdx now points to a space or a null
		argv[*argc] = arg;
		(*argc)++;
		
		// If this is the end of the line
		if (*parseIdx == '\0')
		{
			// Parsing done
			break;
		}

		// if this is not the end we're at a space. skip all the spaces
		*parseIdx = '\0';
		parseIdx++;
		while (*parseIdx == ' ')
		{
			parseIdx++;
		}
		arg = parseIdx;
		if (*parseIdx == '\0')
		{
			// there were trailing spaces at the very end of the cmd line
			break;
		}
	}
}

static void UserInput_ParseInputLineInternal(void)
{
	UserInput_ParseInputLine((char *) &usrInputBuf, &usrCommandArgc, usrCommandArgs, usrInputIdx+1);
}

static bool UserInput_InputPollTimerCallback(struct repeating_timer_t *t)
{
	int c = getchar_timeout_us(0);
	if (c == PICO_ERROR_TIMEOUT)
	{
		return true;
	}
	if (usrInputIdx == 0 && c == USER_INPUT_SPACE_CHAR)
	{
		return true;
	}
	if (c == USER_INPUT_ENTER_CHAR)
	{
		// if (usrInputIdx == 0)
		// {
		// 	return true;
		// }
		usrInputBuf[usrInputIdx] = '\0';
		printf("\n");
		// UserCommand_ProcessInputLine((char *) &usrInputBuf);
		// UserCommand_RequestService();
		UserInput_ParseInputLineInternal();
		commandReady = true;
		usrInputIdx = 0;
	}
	else if (c == USER_INPUT_BKSPACE_CHAR)
	{
		if (usrInputIdx > 0)
		{
			usrInputBuf[usrInputIdx] = '\0';
			printf("\b \b");
			usrInputIdx -= (usrInputIdx > 0 ? 1 : 0);
		}
	}
	else
	{
		usrInputBuf[usrInputIdx] = c;
		usrInputIdx++;
		printf("%c", c);
	}
	return true;
}

void UserInput_StartPollTimer(void)
{
	add_repeating_timer_ms(USER_INPUT_POLL_PERIOD_MS, UserInput_InputPollTimerCallback, NULL, &userInputPollTimer);
}

void UserInput_StopPollTimer(void)
{
	cancel_repeating_timer(&userInputPollTimer);
}

void UserInput_Init(void)
{
	memset((void *) &usrInputBuf, 0x00, sizeof(usrInputBuf));
	// stdio_set_chars_available_callback(UserInput_CharAvailCallback, NULL);
	UserInput_StartPollTimer();
}

void UserInput_Service(void)
{
	if (commandReady)
	{
		UserInput_StopPollTimer();
		UserCommand_ProcessCommand(usrCommandArgc, usrCommandArgs);
		commandReady = false;
		UserInput_StartPollTimer();
	}
}
