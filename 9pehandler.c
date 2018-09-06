#include "9p.h"
#include "9pehandler.h"

struct message
handle(struct message *tmsg)
{
	struct message rmsg;

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
	default:
		rmsg.type = Rerror;
		rmsg.tag = tmsg->tag;
		rmsg.ename = Ebadcmd;
	}
	calc_size(&rmsg);
	return rmsg;
}
