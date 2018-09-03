#include "serialize.h"

uint32_t
deuint32_t(unsigned char *buffer)
{
	uint32_t value = 0;
	value |= buffer[0] << 24;
	value |= buffer[1] << 16;
	value |= buffer[2] << 8;
	value |= buffer[3];
	return value;
}

uint16_t
deuint16_t(unsigned char *buffer)
{
	uint16_t value = 0;
	value |= buffer[0] << 8;
	value |= buffer[1];
	return value;
}

uint8_t
deuint8_t(unsigned char *buffer)
{
	uint8_t value = 0;
	value |= buffer[0];
	return value;
}
