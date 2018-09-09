#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9p.h"
#include "serialize.h"

unsigned char *buffer;

void writeqid(unsigned char *buf, Qid *qid);
char *readstr(unsigned char *buf);

struct message
create_message()
{
	struct message msg;
	msg.size = 0x0;
	msg.type = 0x0;
	msg.tag = 0x0;
	msg.msize = 0x0;
	msg.version = 0x0;
	msg.afid = 0x0;
	msg.uname = 0x0;
	msg.aname = 0x0;
	msg.aqid.type = 0x0;
	msg.aqid.version = 0x0;
	msg.aqid.path = 0x0;
	msg.ename = 0x0;
	msg.oldtag = 0x0;
	msg.fid = 0x0;
	msg.qid.type = 0x0;
	msg.qid.version = 0x0;
	msg.qid.path = 0x0;
	msg.newfid = 0x0;
	msg.nwname = 0x0;
	msg.wname = 0x0;
	msg.nwqid = 0x0;
	msg.wqid = 0x0;
	msg.mode = 0x0;
	msg.iounit = 0x0;
	msg.name = 0x0;
	msg.perm = 0x0;
	msg.offset = 0x0;
	msg.count = 0x0;
	msg.data = 0x0;
	msg.stat = 0x0;
	return msg;
}

unsigned char *
msgtbuf(struct message *msg)
{
	if (msg == 0x0)
		return 0x0;
	if (msg->type < Tversion || msg->type >= Tmax)
		return 0x0;

	buffer = malloc(msg->size);
	memcpy(buffer, &(msg->size), sizeof(msg->size));
	memcpy(buffer+4, &(msg->type), sizeof(msg->type));
	memcpy(buffer+5, &(msg->tag), sizeof(msg->tag));

	switch (msg->type) {
	case Tversion:
	case Rversion:
		memcpy(buffer+7, &(msg->msize), sizeof(msg->msize));
		*(buffer+11) = (uint16_t)strlen(msg->version);
		memcpy(buffer+13, msg->version, sizeof(msg->version)-1);
		return buffer;
	case Rerror:
		*(buffer+7) = (uint16_t)strlen(msg->ename);
		memcpy(buffer+9, &(msg->ename), sizeof(msg->ename));
		return buffer;
	case Rattach:
		writeqid(buffer+7, &(msg->qid));
		return buffer;
	default:
		return 0x0;
	}
}

struct message
buftmsg(unsigned char *buf)
{
	struct message template = create_message();

	template.size = deuint32_t(buf);
	template.type = deuint8_t(buf+4);
	template.tag  = deuint16_t(buf+5);

	switch (template.type) {
	case Tversion:
	case Rversion:
		template.msize = deuint32_t(buf+7);
		template.version = readstr(buf+11);
		break;
	case Tattach:
		template.fid = deuint32_t(buf+7);
		template.afid = deuint32_t(buf+11);
		template.uname = readstr(buf+15);
		template.aname = readstr(buf+17+strlen(template.uname));
		break;
	default:
		break;
	}

	return template;
}

// TODO, yeah, what to do?
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
	case Tattach:
		fprintf(stderr, "Package Information: %d (Tattach)\n"
			"	Size: %u\n"
			"	Tag: %u\n"
			"	Fid: %u\n"
			"	uname: %s\n"
			"	aname: %s\n",
			msg->type,
			msg->size,
			msg->tag,
			msg->fid,
			msg->uname,
			msg->aname);
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

	size = 4; // size
	size += 1; // msgtype
	size += 2; // tag
	switch (msg->type) {
	case Tversion:
	case Rversion:
		size += 4; // msize
		size += 2; // size of string
		size += strlen(msg->version);
		break;
	case Rerror:
		size += 2; // size of string-size
		size += strlen(msg->ename)-1; // ename
		break;
	case Tattach:
		size += 4; // afid
		size += strlen(msg->uname)-1;
		size += strlen(msg->aname)-1;
		break;
	case Rattach:
		size += 13; // aqid
		break;
	default:
		return;
	}

	msg->size = size;
}

void
free_msg(struct message *msg)
{
	if (msg == 0x0)
		return;
	if (msg->type < Tversion || msg->type >= Tmax)
		return;

	/*
	fprintf(stderr, "Freeing: %d\n"
		"	version: %p\n"
		"	ename: %p\n"
		"	uname: %p\n"
		"	aname: %p\n",
		msg->type,
		(void *)msg->version,
		(void *)msg->ename,
		(void *)msg->uname,
		(void *)msg->aname); //DEBUG
	*/

	switch (msg->type) {
	case Tversion:
	case Rversion:
		if (msg->version != 0x0)
			free(msg->version);
		break;
	case Rerror:
		if (msg->ename != 0x0)
			free(msg->ename);
		break;
	case Tattach:
		if (msg->uname != 0x0)
			free(msg->uname);
		if (msg->aname != 0x0)
			free(msg->aname);
		break;
	default:
		break;
	}
}

void
writeqid(unsigned char *buf, Qid *qid)
{
	memcpy(buf, &(qid->type), sizeof(qid->type));
	memcpy(buf + sizeof(qid->type), &(qid->version), sizeof(qid->version));
	memcpy(buf + sizeof(qid->type) + sizeof(qid->version),
	       &(qid->path),
	       sizeof(qid->path));
}

char *
readstr(unsigned char *buf)
{
	uint16_t length = deuint16_t(buf);
	char *str = (char *)malloc(length+1);
	if (str == 0x0) {
		perror("Cannot allocate memory");
		exit(1);
	}
	memcpy(str, buf+sizeof(uint16_t), length);
	*(str+length) = 0x0;
	return str;
}
