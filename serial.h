#ifndef SERIAL_H
#define SERIAL_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


using namespace std;

class Serial {
 protected:
  int tty_fd;
 public:
  // construtor:
  //   path: arquivo de dispositivo da serial
  //   rate: taxa de bps, de acordom com termios.h (ex: B9600, B115200, ...)
  Serial(const char * path, int rate);
  ~Serial();

  // envia size bytes contidos em buffer
  // retorna a quantidade de bytes de fato enviados
  int Write(char * buffer, int size);

  // lê até size bytes, e os armazena em buffer
  // retorna a quantidade de bytes de fato recebidos
  int Read(char * buffer, int size);

  // lê até size bytes, e os armazena em buffer, porém espera no máximo
  // timeout milissegundos
  // retorna a quantidade de bytes de fato recebidos
 // int Read(char * buffer, int size, long timeout);
};

#endif
