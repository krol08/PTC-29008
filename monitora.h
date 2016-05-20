#ifndef MONITORA_H
#define MONITORA_H

#include "serial.h"
#include "tun.h"
#include "enquadramento.h"
#include "arq.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>


using namespace std;

class Monitora {
 
 public:

   Monitora(Serial * dev_serial, tun * dev_tun, Enquadramento * dev_enquadra, Arq * dev_arq);
  ~Monitora();

  void unpack(char * msg, int bytes);

  void sentinela();

 private:

  Serial * mon_serial;
  tun * mon_tun;
  Enquadramento * mon_enquadra;
  Arq * mon_arq;
};

#endif
