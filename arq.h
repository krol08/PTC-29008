#ifndef ARQ_H
#define ARQ_H

#include "enquadramento.h"
//#include "monitora.h"
#include "tun.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


using namespace std;

class Arq {
 public:
  enum tipo_ev {
	TIMEOUT = 0,
	ACK = 1,
	PAYLOAD = 2,
	FRAME = 3,
  };

  Arq(Enquadramento * dev_enquadra, tun * dev_tun);
  ~Arq();

 void unpack(char * msg, int bytes);

 void mef(char * buffer, int bytes, tipo_ev even);

 private:

  enum Estados {
   e0, e1
  };


  int _num_seq_tx;

  int _num_seq_rx;

  char * _dado;

  int _nBytes;

  bool _replay;

  bool _timeout;

  // estado atual da MEF
  Estados  _estado_atual;
  Enquadramento * arq_enquadra;
  tun * arq_tun;

};

#endif
