#ifndef EDITABLE_VALUE_H_
#define EDITABLE_VALUE_H_
#include <stdint.h>
#include <stdbool.h>

#define LONGEST_DATA_TYPE uint64_t // double 
#define COMMON_DATA_TYPE LONGEST_DATA_TYPE

// Below takes the value name itself, not the pointer. the macro will make a pointer from the string 
#define EDITABLE_VALUE(valPtr, type, lowerLimit, upperLimit)  (EditableValue_t) { .name = #valPtr, .valPtr = &valPtr, .type = type, .lowerLimit = lowerLimit, .upperLimit = upperLimit }

typedef enum TYPE_
{
	UINT8_T,
	UINT16_T,
	UINT32_T,
	DOUBLE,
	FLOAT,
	TYPE_MAX
} TYPE;

typedef struct __attribute__((__packed__)) EditableValue_t_
{
	char *name;
	uint8_t *valPtr;
	TYPE type;
	COMMON_DATA_TYPE lowerLimit;
	COMMON_DATA_TYPE upperLimit;
} EditableValue_t;

typedef struct EditableValueList_t_
{
	EditableValue_t *values;
	uint16_t len;
} EditableValueList_t;

void EditableValue_PrintList(EditableValueList_t *list);
bool EditableValue_SetValue(EditableValue_t *v, COMMON_DATA_TYPE *value);
bool EditableValue_SetValueFromString(EditableValue_t *v, char *valStr);
bool EditableValue_FindAndSetValue(EditableValueList_t *l, char *name, COMMON_DATA_TYPE *value);
bool EditableValue_FindAndSetValueFromString(EditableValueList_t *l, char *name, char *valStr);


#endif
