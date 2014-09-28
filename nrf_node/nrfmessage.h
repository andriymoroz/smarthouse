#ifndef NRF_MESSAGE_H
#define NRF_MESSAGE_H

typedef struct {
	int msgtype;
	int msgsubtype;
	union {
		int test;
	} data;
} NrfMessage;

#if 5/*sizeof(NrfMessage)*/ > 32
#error "NrfMessage too long!"
#endif

#endif // NRF_MESSAGE_H
