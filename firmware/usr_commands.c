#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "test_functionality.h"
#include "addr_led_driver.h"

#define ASSERT_ARGS(argcExpected) {if (argc < argcExpected) {printf("Bad args! argc %d expected %d\n", argc, argcExpected); return;}}
#define BAD_ARG() {printf("Bad arg!\n"); UserCommand_PrintCommand(argc, argv);}

UserCommand_t userCommands[] = {
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
	{"test", UserCommand_Test, "test"},
	{"led", UserCommand_LedSet, "Set Led"},
	{"loopback", UserCommand_Loopback, "Loopback"},
	{"misc", UserCommand_Misc, "Misc programs"},
	{"aled", UserCommand_AddrLed, "Addr led commands"}
};

static void UserCommand_PrintCommand(uint16_t argc, char **argv)
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

void UserCommand_AddrLed(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	// uint16_t level = atoi(argv[1]);
	// AddrLedDriver_SetLevel(level);
	if (strcmp(argv[1], "clear") == 0)
	{
		AddrLedDriver_Clear();
	}
	else if (strcmp(argv[1], "set") == 0)
	{
		// aled set <pos> <x> <y> <r> <g> <b>
		ASSERT_ARGS(8);
		Position_e pos = NUM_SIDES;
		if (strcmp(argv[2], "n") == 0)
		{
			pos = NORTH;
		}
		else if (strcmp(argv[2], "e") == 0)
		{
			pos = EAST;
		}
		else if (strcmp(argv[2], "s") == 0)
		{
			pos = SOUTH;
		}
		else if (strcmp(argv[2], "w") == 0)
		{
			pos = WEST;
		}
		else if (strcmp(argv[2], "t") == 0)
		{
			pos = TOP;
		}
		else
		{
			printf("BAD SIDE DESCRIPTOR! %s\n", argv[0]);
			return;
		}
		uint8_t x = atoi(argv[3]);
		uint8_t y = atoi(argv[4]);
		uint8_t r = atoi(argv[5]);
		uint8_t g = atoi(argv[6]);
		uint8_t b = atoi(argv[7]);
		printf("Setting pixel %s %d %d to %d %d %d\n", AddrLedDriver_GetPositionString(pos), x, y, r, g, b);
		AddrLedDriver_SetPixelRgbInPanel(pos, x, y, r, g, b);
	}
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
