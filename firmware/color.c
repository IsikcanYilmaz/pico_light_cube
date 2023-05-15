#include "color.h"

Color_t Color_CreateFromRgb(uint8_t r, uint8_t g, uint8_t b)
{
	Color_t c = {.red = r, .green = g, .blue = b};
	// TODO deal with HSV
	return c;
}

Color_t Color_CreateFromHsv(double h, double s, double v)
{
}

void Color_SetRgb(uint8_t r, uint8_t g, uint8_t b)
{
}

void Color_SetHsv(double h, double s, double v)
{
}
