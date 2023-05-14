#define USER_COMMAND_MAX_ARGS 16

typedef struct UserCommand_s
{
	char *command;
	void (*fn)(uint8_t argc, char **argv);
	char *helptext;
} UserCommand_t;

extern UserCommand_t userCommands[];

void UserCommand_ProcessInputLine(char *buf);
void UserCommand_ProcessCommand(uint16_t argc, char **argv);
void UserCommand_Reset(uint8_t argc, char **argv);
void UserCommand_Test(uint8_t argc, char **argv);
void UserCommand_LedSet(uint8_t argc, char **argv);
void UserCommand_Loopback(uint8_t argc, char **argv);
void UserCommand_Wifi(uint8_t argc, char **argv);
void UserCommand_Oled(uint8_t argc, char **argv);
void UserCommand_Misc(uint8_t argc, char **argv);
void UserCommand_AddrLed(uint8_t argc, char **argv);
