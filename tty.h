/* s2e.h - Serial To Ethernet header */

/* Copyright (c) 2008, Nissin Systems Co., Ltd. All rights reserved. */

/*
modification history
--------------------
01b,15jan10,nss  added SSH/SSL.
01a,05aug08,nss  written.
*/

#ifndef _S2E_H
#define _S2E_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>

#ifndef B230400
  #define  B230400  0010003
#endif

#define	TTY_BUF_SIZE	2048	/* size of tty buffer */

typedef struct
{
	char tty_device[16];			/* device name */
	long tty_baudrate;			/* baudrate */
	int tty_parity;				/* parity <0:none 1:odd 2: even> */
	int tty_stopbit;			/* stop bit <1 or 2> */
	int tty_length;				/* bit length <from 5 to 8> */
	int tty_flow;				/* flow control <0:none 1:soft 2: hard> */
	int tty_timeout;			/* receive timeout for transmition trigger */
	int tty_dma;				/* DMA transfer mode <0:disable 1:enable> */
	int tty_rs485;				/* RS-485 mode <0:disable 1:half 2:full 3:t-half 4:t-full> */

	int tty_size;				/* size for transmition trigger */
	char tty_delim_code[3];			/* delimiter code for transmition trigger */
	char tty_delim_len;			/* length of delimiter code */

	int tty_fd;				/* tty descripter */

	char *tty_buffer;			/* pointer to tty buffer */
	int tty_bufsz;				/* size of tty buffer */

	unsigned long tty_rcvd;			/* received data bytes */
	unsigned long tty_sent;			/* sent data bytes */
}tTTY_CONF;

extern int tty_open (tTTY_CONF *conf);
extern int tty_close(tTTY_CONF *conf);
extern int tty_read (tTTY_CONF *conf, char *buffer, int size);
extern int tty_write (tTTY_CONF *conf, char *buffer, int size);


#endif	/* _S2E_H */
