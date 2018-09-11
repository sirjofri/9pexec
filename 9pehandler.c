#include "9p.h"
#include "9pehandler.h"

struct message
handle(struct message *tmsg)
{
	struct message rmsg = create_message();
	uint32_t mode;

	if (tmsg == 0x0) {
		rmsg.type = Rerror;
		rmsg.tag = tmsg->tag;
		rmsg.ename = Ebadcmd;
		calc_size(&rmsg);
		return rmsg;
	}

	switch (tmsg->type) {
	case Tversion:
		rmsg.type = Rversion;
		rmsg.tag = tmsg->tag;
		rmsg.msize = MSIZE;
		rmsg.version = VERSION9P;
		break;
	case Tattach:
		rmsg.type = Rattach;
		rmsg.tag = tmsg->tag;
		rmsg.qid.version = 0x0;
		rmsg.qid.path = 0x0;
		mode = DMDIR >> 24;
		rmsg.qid.type = mode;
		break;
	case Tclunk:
		rmsg.type = Rclunk;
		rmsg.tag = tmsg->tag;
		break;
	case Tflush:
		rmsg.type = Rflush;
		rmsg.tag = tmsg->tag;
		break;
	default:
		rmsg.type = Rerror;
		rmsg.tag = tmsg->tag;
		rmsg.ename = Ebadcmd;
	}
	calc_size(&rmsg);
	return rmsg;
}
