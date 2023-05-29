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
			printf("val:%d lowerLimit:%d upperLimit:%d\n", * (uint8_t *) (v->valPtr), v->ll.u8, v->ul.u8);
			break;
		}
		case UINT16_T:
		{
			printf("val:%d lowerLimit:%d upperLimit:%d\n", * (uint16_t *) (v->valPtr),  v->ll.u16, v->ul.u16);
			break;
		}
		case UINT32_T:
		{
			printf("val:%d lowerLimit:%d upperLimit:%d\n", * (uint32_t *) (v->valPtr), v->ll.u32, v->ul.u32);
			break;
		}
		case DOUBLE:
		{
			printf("val:%f lowerLimit:%f upperLimit:%f\n", * (double *) (v->valPtr), v->ll.d, v->ul.d);
			break;
		}
		case FLOAT:
		{
			printf("val:%f lowerLimit:%f upperLimit:%f\n", * (float *) (v->valPtr), v->ll.f, v->ul.f);
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

bool EditableValue_SetValue(EditableValue_t *editable, union EightByteData_u *value)
{
	// printf("%s\n", __FUNCTION__);
	TYPE t = editable->type;
	bool ret = true;
	switch(t)
	{
		case UINT8_T:
		{
			if (value->u8 < editable->ll.u8 || value->u8 > editable->ul.u8)
			{
				printf("Val %d not in range! %d %d\n", value->u8, editable->ll.u8, editable->ul.u8);
				ret = false;
				break;
			}
			* (uint8_t *) (editable->valPtr) = value->u8;
			break;
		}
		case UINT16_T:
		{
			if (value->u16 < editable->ll.u16 || value->u16 > editable->ul.u16)
			{
				printf("Val %d not in range! %d %d\n", value->u16, editable->ll.u16, editable->ul.u16);
				ret = false;
				break;
			}
			* (uint16_t *) (editable->valPtr) = value->u16;
			break;
		}
		case UINT32_T:
		{
			if (value->u32 < editable->ll.u32 || value->u32 > editable->ul.u32)
			{
				printf("Val %d not in range! %d %d\n", value->u32, editable->ll.u32, editable->ul.u32);
				ret = false;
				break;
			}
			* (uint32_t *) (editable->valPtr) = value->u32;
			break;
		}
		case DOUBLE:
		{
			if (value->d < editable->ll.d || value->d > editable->ul.d)
			{
				printf("Val %f not in range! %f %f\n",value->d , editable->ll.u32, editable->ul.u32);
				ret = false;
				break;
			}
			* (uint32_t *) (editable->valPtr) = value->d;
			break;
		}
		case FLOAT:
		{
			if (value->f < editable->ll.f || value->f > editable->ul.f)
			{
				printf("Val %d not in range! %d %d\n", value->f, editable->ll.f, editable->ul.f);
				ret = false;
				break;
			}
			* (float *) (editable->valPtr) = value->f;
			break;
		}
		default:
		{
			return false;
		}
	}
	// printf("%s DONE\n", __FUNCTION__);
	return ret;
}

bool EditableValue_SetValueFromString(EditableValue_t *v, char *valStr)
{
	// printf("%s\n", __FUNCTION__);
	union EightByteData_u tmpVal;
	switch(v->type)
	{
		case UINT8_T:
		{
			tmpVal.u8 = (uint8_t) atoi(valStr);
			return EditableValue_SetValue(v, &tmpVal);
			break;
		}
		case UINT16_T:
		{
			tmpVal.u16 = (uint16_t) atoi(valStr);
			return EditableValue_SetValue(v, &tmpVal);
			break;
		}
		case UINT32_T:
		{
			tmpVal.u32 = (uint32_t) atoi(valStr);
			return EditableValue_SetValue(v, &tmpVal);
			break;
		}
		case DOUBLE:
		{
			tmpVal.d = (double) atof(valStr);
			return EditableValue_SetValue(v, &tmpVal);
			break;
		}
		case FLOAT:
		{
			tmpVal.f = (float) atof(valStr);
			return EditableValue_SetValue(v, &tmpVal);
			break;
		}
		default:
		{
			return false;
		}
	}
	// printf("%s DONE\n", __FUNCTION__);
}

bool EditableValue_FindAndSetValueFromString(EditableValueList_t *l, char *name, char *valStr)
{
	EditableValue_t *v;
	bool found = false;
	for (uint16_t i = 0; i < l->len; i++)
	{
		// printf("Looking for %s.. %s\n", name, l->values[i].name);
		if (strcmp(l->values[i].name, name) == 0)
		{
			v = &(l->values[i]);
			found = true;
			break;
		}
	}
	if (!found)
	{
		printf("%s not found in value list!\n");
		return found;
	}

	return EditableValue_SetValueFromString(v, valStr);
}
