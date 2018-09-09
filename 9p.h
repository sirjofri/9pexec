#include <inttypes.h>

/* Protocol statics */
#define VERSION9P "9P2000"
#define NOTAG 0
#define NOFID (uint32_t)0
#define MSIZE (1024)

/* Protocol msgtypes */

typedef
enum msgtype {
	Tversion = 100,
	Rversion,
	Tauth = 102,
	Rauth,
	Tattach = 104,
	Rattach,
	Terror = 106, /* illegal */
	Rerror,
	Tflush = 108,
	Rflush,
	Twalk = 110,
	Rwalk,
	Topen = 112,
	Ropen,
	Tcreate = 114,
	Rcreate,
	Tread = 116,
	Rread,
	Twrite = 118,
	Rwrite,
	Tclunk = 120,
	Rclunk,
	Tremove = 122,
	Rremove,
	Tstat = 124,
	Rstat,
	Twstat = 126,
	Rwstat,
	Tmax,
} msgtype;

/* Error messages */
#define Enomem    "out of memory"
#define Eperm     "permission denied"
#define Enodev    "no free devices"
#define Ehungup   "write to hungup channel"
#define Eexist    "file exists"
#define Enonexist "file does not exist"
#define Ebadcmd   "bad command"
#define Ebadarg   "bad arg in system call"
#define Enofid    "no such fid"
#define Enotdir   "not a directory"
#define Eopen     "already open"
#define Ebadfid   "bad fid"

/* File mode */
#define DMDIR     0x80000000 // directory
#define DMAPPEND  0x40000000 // append only
#define DMEXCL    0x20000000 // exclusive user
#define DMTMP     0x04000000 // temporary

/* Protocol specification */

/* generic qid */
typedef struct Qid {
	uint8_t type; // type of file
	uint32_t version; // increment after modification
	uint64_t path; // unique path in the hierarchy
} Qid;

/* generic 9p message */
struct message {
	uint32_t size;
	msgtype type;
	uint16_t tag;
	/* trversion */
	uint32_t msize;
	char *version;
	/* tauth */
	uint32_t afid;
	char *uname;
	char *aname;
	/* rauth */
	Qid aqid;
	/* rerror */
	char *ename;
	/* tflush */
	uint16_t oldtag;
	/* tattach */
	uint32_t fid;
	/* rattach */
	Qid qid;
	/* twalk */
	uint32_t newfid;
	uint16_t nwname;
	char **wname; /* nwname * ( char *wname ) */
	/* rwalk */
	uint16_t nwqid;
	Qid *wqid; /* nwqid * ( char *wqid ) */
	/* topen */
	char mode;
	/* ropen */
	uint32_t iounit;
	/* tcreate */
	char *name;
	uint32_t perm;
	/* tread */
	uint64_t offset;
	uint32_t count;
	/* rread */
	char *data; // length: count
	/* rstat */
	char *stat; // mixed
};

#ifdef INFORMATION_PLEASE_IGNORE
/* version */
struct Tversion {
	char size[4];
	char type; // = TVERSION;
	char tag[2];
	char msize[4];
	char *version;
};

struct Rversion {
	char size[4];
	char type; // = RVERSION;
	char tag[2];
	char msize[4];
	char *version;
};

/* auth */
struct Tauth {
	char size[4];
	char type; // = TAUTH;
	char tag[2];
	char afid[4];
	char *uname;
	char *aname;
};

struct Rauth {
	char size[4];
	char type; // = RAUTH;
	char tag[2];
	char aquid[13];
};

/* error */
struct Rerror {
	char size[4];
	char type; // = RERROR;
	char tag[2];
	char *ename;
};

/* flush message */
struct Tflush {
	char size[4];
	char type; // = TFLUSH;
	char tag[2];
	char oldtag[2];
};

struct Rflush {
	char size[4];
	char type; // = RFLUSH;
	char tag[2];
};

/* attach connection */
struct Tattach {
	char size[4];
	char type; // = TATTACH;
	char tag[2];
	char fid[4];
	char afid[4];
	char *uname;
	char *aname;
};

struct Rattach {
	char size[4];
	char type; // = RATTACH;
	char tag[2];
	char qid[13];
};

/* descend dir hierarchy */
struct Twalk {
	char size[4];
	char type; // = TWALK;
	char tag[2];
	char fid[4];
	char newfid[4];
	char nwname[2];
	char **wname; /* nwname * ( char *wname ) */
};

struct Rwalk {
	char size[4];
	char type; // = RWALK;
	char tag[2];
	char nwqid[2];
	char *wqid[13]; /* nwqid * ( char *wqid ) */
};

/* open file/directory (prepare IO) */
struct Topen {
	char size[4];
	char type; // = TOPEN;
	char tag[2];
	char fid[4];
	char mode;
};

struct Ropen {
	char size[4];
	char type; // = ROPEN;
	char tag[2];
	char qid[13];
	char iounit[4];
};

/* create file fid, prepare IO */
struct Tcreate {
	char size[4];
	char type; // = TCREATE;
	char tag[2];
	char fid[4];
	char *name;
	char perm[4];
	char mode;
};

struct Rcreate {
	char size[4];
	char type; // = RCREATE;
	char tag[2];
	char qid[13];
	char iounit[4];
};

/* read from file fid */
struct Tread {
	char size[4];
	char type; // = TREAD;
	char tag[2];
	char fid[4];
	char offset[8];
	char count[4];
};

struct Rread {
	char size[4];
	char type; // = RREAD;
	char tag[2];
	char count[4];
	char data[count];
};

/* write to file fid */
struct Twrite {
	char size[4];
	char type; // = TWRITE;
	char tag[2];
	char fid[4];
	char offset[8];
	char count[4];
	char data[count];
};

struct Rwrite {
	char size[4];
	char type; // = RWRITE;
	char tag[2];
	char count[4];
};

/* forget about a fid */
struct Tclunk {
	char size[4];
	char type; // = TCLUNK;
	char tag[2];
	char fid[4];
};

struct Rclunk {
	char size[4];
	char type; // = RCLUNK;
	char tag[2];
};

/* remove file fid */
struct Tremove {
	char size[4];
	char type; // = TREMOVE;
	char tag[2];
	char fid[4];
};

struct Rremove {
	char size[4];
	char type; // = RREMOVE;
	char tag[2];
};

/* read stats of fid */
struct Tstat {
	char size[4];
	char type; // = TSTAT;
	char tag[2];
	char fid[4];
};

struct Rstat {
	char size[4];
	char type; // = RSTAT;
	char tag[2];
	char stat[n];
};

/* set stats of fid */
struct Twstat {
	char size[4];
	char type; // = TSTAT;
	char tag[2];
	char fid[4];
	char stat[n];
};

struct Rwstat {
	char size[4];
	char type; // = RSTAT;
	char tag[2];
};
#endif

/*
 * functions
 */

/* read 9pstr struct pointer, transform it into a valid network buffer, return
 * pointer to that buffer. */
unsigned char *msgtbuf(struct message *msg);

/* reverse of msgtbuf */
struct message buftmsg(unsigned char *buf);

/* prepares struct. check values and fill in if needed */
void prepare_msg(struct message *msg);

/* dump */
void msgdump(struct message *msg);

void calc_size(struct message *msg);

/* use this instead of free(msg); */
void free_msg(struct message *msg);

struct message create_message();
