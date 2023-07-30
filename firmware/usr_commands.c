#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "test_functionality.h"
#include "addr_led_driver.h"
#include "animation_manager.h"


UserCommand_t userCommands[] = {
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
	{"test", UserCommand_Test, "test"},
	{"led", UserCommand_LedSet, "Set Led"},
	{"loopback", UserCommand_Loopback, "Loopback"},
	{"misc", UserCommand_Misc, "Misc programs"},
	{"aled", AddrLedDriver_TakeUsrCommand, "Addr led commands"},
	{"anim", AnimationMan_TakeUsrCommand, "Animation commands"},
	{"help", UserCommand_HelpText, "Help Text"}
};

void UserCommand_PrintCommand(uint16_t argc, char **argv)
{
	printf("COMMAND: ");
	for (uint16_t i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
}

void UserCommand_ProcessCommand(uint16_t argc, char **argv)
{
	// We have the command and args parsed now. do a lookup to see
	// which command is entered and call its function
	for (uint16_t i = 0; i < (sizeof(userCommands) / sizeof(userCommands[0])); i++)
	{
		if (strcmp(userCommands[i].command, argv[0]) == 0)
		{
			// printf("COMMAND %s argc %d\n", argv[0], argc); // TODO
			userCommands[i].fn(argc, argv);
			return;
		}
	}
	printf("Command not found\n");
}

// COMMANDS // 
void UserCommand_Reset(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	if (strcmp(argv[1], "boot") == 0)
	{
		softwareReset(true);
		return;
	}
	else if (strcmp(argv[1], "sw") == 0)
	{
		softwareReset(false);
		return;
	}
}

void UserCommand_Test(uint8_t argc, char **argv)
{
	toggleLed();
}

void UserCommand_LedSet(uint8_t argc, char **argv)
{
	bool boolArg = (atoi(argv[1]) > 0);
	setLed(boolArg);
}

void UserCommand_Loopback(uint8_t argc, char **argv)
{
	for (uint16_t i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
}

void UserCommand_Misc(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	
	// args
	
}
