/*
 * serial.cpp
 *
 *  Created on: 13 de abr de 2016
 *      Author: root
 */

#include "serial.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


Serial::Serial(const char * path, int rate) {

    struct termios tio;

  tty_fd=open(path, O_RDWR|O_NOCTTY);
  tcgetattr(tty_fd, &tio);
  bzero(&tio,sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag=0;
  tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=5;

  cfsetospeed(&tio,rate);
  cfsetispeed(&tio,rate);

  tcsetattr(tty_fd,TCSANOW,&tio);

}

Serial::~Serial() {
  sleep(1);
  close(this->tty_fd);
}

int Serial::Read(char * buffer, int bytes) {


	return::read(tty_fd,buffer,bytes);

}

int Serial::Write(char * buffer, int bytes) {  // Camada de enlace.


   return ::write(this->tty_fd, buffer, bytes);  //write da camada física.
}

/*int Serial::read(char * buffer, int size, long timeout) {
  struct timeval tv;
  fd_set rfd;

  FD_ZERO(&rfd);
  FD_SET(tty_fd, &rfd);
  tv.tv_sec = timeout/1000;
  tv.tv_usec = (timeout%1000) * 1000;
  int n = select(tty_fd+1, &rfd, NULL, NULL, &tv);
  if (not n) return 0;
  return ::read(tty_fd, buffer, size);

}*/

