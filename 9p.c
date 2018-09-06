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
		*(buffer+11) = (uint16_t)strlen(msg->version);
		memcpy(buffer+13, msg->version, sizeof(msg->version)-1);
		return buffer;
	case Rerror:
		memcpy(buffer, &(msg->size), sizeof(msg->size));
		memcpy(buffer+4, &(msg->type), sizeof(msg->type));
		memcpy(buffer+5, &(msg->tag), sizeof(msg->tag));
		*(buffer+7) = (uint16_t)strlen(msg->ename);
		memcpy(buffer+9, &(msg->ename), sizeof(msg->ename));
		return buffer;
	default:
		return 0x0;
	}
}

struct message
buftmsg(unsigned char *buf)
{
	struct message template;
	uint16_t strsize;
	char *version;

	template.size = deuint32_t(buf);
	template.type = deuint8_t(buf+4);
	template.tag = deuint16_t(buf+5);

	switch (template.type) {
	case Tversion:
		template.msize = deuint32_t(buf+7);
		strsize = deuint16_t(buf+11);
		version = (char *)malloc(strsize+1);
		if (version == 0x0) {
			perror("Cannot allocate memory");
			exit(1);
		}
		memcpy(version, buf+13, strsize);
		*(version+strsize) = 0x0;
		template.version = version;
		break;
	case Rversion:
		template.msize = deuint32_t(buf+7);
		strsize = deuint16_t(buf+11);
		version = (char *)malloc(strsize+1);
		if (version == 0x0) {
			perror("Cannot allocate memory");
			exit(1);
		}
		memcpy(version, buf+13, strsize);
		*(version+strsize) = 0x0;
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
		msg->size = (length + 13)*sizeof(char);
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
		fprintf(stderr, "Package Information: %d (Tversion) \n"
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
	case Rversion:
		fprintf(stderr, "Package Information: %d (Rversion)\n"
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
		size = 4; // size-field
		size += 1; // msgtype
		size += 2; // tag
		size += 4; // msize
		size += 2; // size of string
		size += strlen(msg->version);
		break;
	case Rerror:
		size = 4; // size-field
		size += 1; // msgtype
		size += 2; // tag
		size += 2; // size of string-size
		size += strlen(msg->ename)-1; // ename
		break;
	default:
		return;
	}

	msg->size = size;
}
