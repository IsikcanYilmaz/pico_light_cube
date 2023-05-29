#include "editable_value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void EditableValue_PrintValue(EditableValue_t *v)
{
	printf("editable val: %s type:%d ", v->name, v->type);
	switch(v->type)
	{
		case UINT8_T:
		{
			printf("val:%d lowerLimit:%d upperLimit:%d\n", * (uint8_t *) (v->valPtr), * (uint8_t *) &v->lowerLimit, * (uint8_t *) &v->upperLimit);
			break;
		}
		case UINT16_T:
		{
			printf("val:%d lowerLimit:%d upperLimit:%d\n", * (uint16_t *) (v->valPtr), (uint16_t) v->lowerLimit, (uint16_t) v->upperLimit);
			break;
		}
		case UINT32_T:
		{
			printf("val:%d lowerLimit:%d upperLimit:%d\n", * (uint32_t *) (v->valPtr), (uint32_t) v->lowerLimit, (uint32_t) v->upperLimit);
			break;
		}
		case DOUBLE:
		{
			printf("val:%f lowerLimit:%f upperLimit:%f\n", * (double *) (v->valPtr), (double) v->lowerLimit, (double) v->upperLimit);
			break;
		}
		case FLOAT:
		{
			printf("val:%f lowerLimit:%f upperLimit:%f\n", * (float *) (v->valPtr), (float) v->lowerLimit, (float) v->upperLimit);
			break;
		}
	}

	// Hexdump
	// char *b = (char *) v;
	// for (int i = 0; i < sizeof(EditableValue_t); i++)
	// {
	// 	printf("%02x ", *(b+i));
	// 	if (i % 16 == 0 && i != 0) printf("\n");
	// }
	// printf("\n");
}

void EditableValue_PrintList(EditableValueList_t *list)
{
	printf("List len: %d\n", list->len);
	for (int i = 0; i < list->len; i++)
	{
		EditableValue_t *v = &(list->values[i]);
		EditableValue_PrintValue(v);
	}
}

bool EditableValue_SetValue(EditableValue_t *v, COMMON_DATA_TYPE *value)
{
	printf("%s\n", __FUNCTION__);
	TYPE t = v->type;
	bool ret = true;
	switch(t)
	{
		case UINT8_T:
		{
			uint8_t newVal = * (uint8_t *) (value);
			if (newVal < (uint8_t) v->lowerLimit || newVal > (uint8_t) v->upperLimit)
			{
				printf("Val %d not in range! %d %d\n", (uint8_t) newVal, (uint8_t) v->lowerLimit, (uint8_t) v->upperLimit);
				ret = false;
				break;
			}
			* (uint8_t *) (v->valPtr) = newVal;
			break;
		}
		case UINT16_T:
		{
			uint16_t newVal = * (uint16_t *) (value);
			if (newVal < (uint16_t) v->lowerLimit || newVal > (uint16_t) v->upperLimit)
			{
				printf("Val %d not in range! %d %d\n", (uint16_t) newVal, (uint16_t) v->lowerLimit, (uint16_t) v->upperLimit);
				ret = false;
				break;
			}
			* (uint16_t *) (v->valPtr) = newVal;
			break;
		}
		case UINT32_T:
		{
			uint32_t newVal = * (uint32_t *) (value);
			if (newVal < (uint32_t) v->lowerLimit || newVal > (uint32_t) v->upperLimit)
			{
				printf("Val %d not in range! %d %d\n", (uint32_t) newVal, (uint32_t) v->lowerLimit, (uint32_t) v->upperLimit);
				ret = false;
				break;
			}
			* (uint32_t *) (v->valPtr) = newVal;
			break;
		}
		case DOUBLE:
		{
			double newVal = * (double *) (value);
			if (newVal < (double) v->lowerLimit || newVal > (double) v->upperLimit)
			{
				printf("Val %f not in range! %f %f\n", (double) newVal, (double) v->lowerLimit, (double) v->upperLimit);
				ret = false;
				break;
			}
			* (double *) (v->valPtr) = newVal;
			break;
		}
		case FLOAT:
		{
			float newVal = * (float *) (value);
			if (newVal < (float) v->lowerLimit || newVal > (float) v->upperLimit)
			{
				printf("Val %f not in range! %f %f\n", (float) newVal, (float) v->lowerLimit, (float) v->upperLimit);
				ret = false;
				break;
			}
			* (float *) (v->valPtr) = newVal;
			break;
		}
		default:
		{
			return false;
		}
	}
	printf("%s DONE\n", __FUNCTION__);
	return ret;
}

bool EditableValue_SetValueFromString(EditableValue_t *v, char *valStr)
{
	printf("%s\n", __FUNCTION__);
	switch(v->type)
	{
		case UINT8_T:
		{
			uint8_t tmpVal = (uint8_t) atoi(valStr);
			return EditableValue_SetValue(v, (COMMON_DATA_TYPE *) &tmpVal);
			break;
		}
		case UINT16_T:
		{
			uint16_t tmpVal = (uint16_t) atoi(valStr);
			return EditableValue_SetValue(v, (COMMON_DATA_TYPE *) &tmpVal);
			break;
		}
		case UINT32_T:
		{
			uint32_t tmpVal = (uint32_t) atoi(valStr);
			return EditableValue_SetValue(v, (COMMON_DATA_TYPE *) &tmpVal);
			break;
		}
		case DOUBLE:
		{
			double tmpVal = (double) atof(valStr);
			return EditableValue_SetValue(v, (COMMON_DATA_TYPE *) &tmpVal);
			break;
		}
		case FLOAT:
		{
			float tmpVal = (float) atof(valStr);
			return EditableValue_SetValue(v, (COMMON_DATA_TYPE *) &tmpVal);
			break;
		}
		default:
		{
			return false;
		}
	}
	printf("%s DONE\n", __FUNCTION__);
}

bool EditableValue_FindAndSetValueFromString(EditableValueList_t *l, char *name, char *valStr)
{
	EditableValue_t *v;
	bool found = false;
	for (uint16_t i = 0; i < l->len; i++)
	{
		printf("Looking for %s.. %s\n", name, l->values[i].name);
		if (strcmp(l->values[i].name, name) == 0)
		{
			v = &(l->values[i]);
			found = true;
			break;
		}
	}
	if (!found)
	{
		printf("%s could not be found in value list!\n");
		return found;
	}

	return EditableValue_SetValueFromString(v, valStr);
}
