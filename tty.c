/* tty.c - tty library */

/* Copyright (c) 2008, Nissin Systems Co., Ltd. All rights reserved. */

/*
modification history
--------------------
01a,05aug08,nss  written.
*/

#include "tty.h"

int tty_open (tTTY_CONF *conf)
{
    int fd = -1;
    int res;
    struct termios oldtio, newtio;

    //if (debug)
    //printf ("tty_open() ... \n");

    if((fd = open(conf->tty_device, O_RDWR | O_NOCTTY | O_NONBLOCK))==-1){
	printf("E: tty_open \n");
	//PERROR("tty_open: open()");
	return -1;
    }

    newtio.c_cflag = (CLOCAL | CREAD);
    newtio.c_iflag = (IGNPAR);
    newtio.c_oflag = 0;

    switch (conf->tty_baudrate) {
		case 300:	newtio.c_cflag |= B300; break;
		case 600:	newtio.c_cflag |= B600; break;
		case 1200:	newtio.c_cflag |= B1200; break;
		case 2400:	newtio.c_cflag |= B2400; break;
		case 4800:	newtio.c_cflag |= B4800; break;
		case 19200:	newtio.c_cflag |= B19200; break;
		case 38400:	newtio.c_cflag |= B38400; break;
		case 57600:	newtio.c_cflag |= B57600; break;
		case 115200:	newtio.c_cflag |= B115200; break;
		case 230400:	newtio.c_cflag |= B230400; break;
		case 921600:    newtio.c_cflag |= B921600; break;
		case 9600:
		default:	newtio.c_cflag |= B9600; break;
    }

    switch (conf->tty_parity) {
		case 1: newtio.c_cflag |= PARENB | PARODD; break;
		case 2: newtio.c_cflag |= PARENB; break;
		case 0:
		default: newtio.c_cflag &= ~(PARENB | PARODD); break;
    }

    switch (conf->tty_stopbit) {
		case 2: newtio.c_cflag |= CSTOPB; break;
		case 1:
		default: newtio.c_cflag &= ~CSTOPB; break;
    }

    switch (conf->tty_length) {
      case 5: newtio.c_cflag |= CS5; break;
      case 6: newtio.c_cflag |= CS6; break;
      case 7: newtio.c_cflag |= CS7; break;
      case 8:
      default: newtio.c_cflag |= CS8; break;
    }

    switch (conf->tty_flow) {
		case 1: newtio.c_iflag |= IXON | IXOFF | IXANY; break;
		case 2: newtio.c_cflag |= CRTSCTS; break;
		case 0:
		default:
			newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
			newtio.c_cflag &= ~CRTSCTS;
			break;
    }

    newtio.c_lflag &= ~(ICANON | ECHO | ISIG);

    newtio.c_cc[VMIN] = 1;
    newtio.c_cc[VTIME] = 0;

    newtio.c_cc[VSTOP] = 0x13;
    newtio.c_cc[VSTART] = 0x11;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW, &newtio);

    conf->tty_fd = fd;

    return 0;
}

int tty_close(tTTY_CONF *conf)
{
    //if (debug)
    //printf ("tty_close() ... \n");

    if (0 < conf->tty_fd)
	close (conf->tty_fd);
    conf->tty_fd = -1;

    return 0;
}

int tty_read (tTTY_CONF *conf, char *buffer, int size)
{
  int ret;

  ret = read (conf->tty_fd, buffer, size);

  //if (debug)
  //printf ("tty_read(0x%x,%d) ... (%d)\n", (unsigned) buffer, size, ret);

  if (0 < ret)
	  conf->tty_rcvd += ret;

  return ret;
}

int tty_write (tTTY_CONF *conf, char *buffer, int size)
{
    int ret;

    ret = write (conf->tty_fd, buffer, size);

    //if (debug)
    printf ("tty_write(0x%x,%d) ... (%d)\n", (unsigned) buffer, size, ret);

    if (0 < ret)
	conf->tty_sent += ret;

    return ret;
}
