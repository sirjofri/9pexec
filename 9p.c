#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9p.h"
#include "serialize.h"

unsigned char *buffer;

unsigned char *
msgtbuf(struct message *msg)
{
	if (msg == 0x0)
		return 0x0;

	buffer = malloc(msg->size);
	switch (msg->type) {
	case Tversion:
	case Rversion:
		memcpy(buffer, &(msg->size), sizeof(msg->size));
		memcpy(buffer+4, &(msg->type), sizeof(msg->type));
		memcpy(buffer+5, &(msg->tag), sizeof(msg->tag));
		memcpy(buffer+7, &(msg->msize), sizeof(msg->msize));
		memcpy(buffer+11, msg->version, sizeof(msg->version)-1);
		return buffer;
	case Rerror:
		memcpy(buffer, &(msg->size), sizeof(msg->size));
		memcpy(buffer+4, &(msg->type), sizeof(msg->type));
		memcpy(buffer+5, &(msg->tag), sizeof(msg->tag));
		memcpy(buffer+7, &(msg->ename), sizeof(msg->ename));
	default:
		return 0x0;
	}
}

struct message
buftmsg(unsigned char *buf)
{
	struct message template;

	template.size = deuint32_t(buf);
	template.type = deuint8_t(buf+4);
	template.tag = deuint16_t(buf+5);

	switch (template.type) {
	case Tversion:
	case Rversion:
		template.msize = deuint32_t(buf+7);
		char *version = malloc(template.size - 11);
		memcpy(version, buf+11, template.size-11);
		template.version = version;
		break;
	default:
		break;
	}

	return template;
}

void
prepare_msg(struct message *msg)
{
	if (msg == 0x0)
		return;

	switch (msg->type) {
	case Tversion:
	case Rversion:
		msg->tag = NOTAG;
		if (!msg->version) {
			msg->version = VERSION9P;
		}
		int length = strlen(msg->version);
		msg->size = (length + 11)*sizeof(char);
		break;
	default:
		return;
	}
}

void
msgdump(struct message *msg)
{
	if (msg == 0x0)
		return;

	switch (msg->type) {
	case Tversion:
	case Rversion:
		fprintf(stderr, "Package Information: %d (T|Rversion)\n"
			"	Size: %u\n"
			"	MSize: %u\n"
			"	Tag: %u\n"
			"	Version: %s\n",
			msg->type,
			msg->size,
			msg->msize,
			msg->tag,
			msg->version);
		break;
	default:
		fprintf(stderr, "unhandled: %d\n", msg->type);
	}
}

void
calc_size(struct message *msg)
{
	uint32_t size = 0;

	if (msg == 0x0)
		return;
	if (msg->type < Tversion || msg->type >= Tmax)
		return;

	switch (msg->type) {
	case Tversion:
	case Rversion:
		return;
	case Rerror:
		size = 4; // size-field
		size += 2; // tag
		size += sizeof(msg->ename)-1; // ename
		return;
	default:
		return;
	}
}
