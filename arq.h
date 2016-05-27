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
	POLL = 4
  };

  Arq(Enquadramento * dev_enquadra, tun * dev_tun);
  ~Arq();

 void unpack(char * msg, int bytes);

 void mef(char * buffer, int bytes, tipo_ev even);

 bool veriTam(int bytes);

 int tipEther_envia(char * buffer, char * quadro, int bytes);

 int tipEther_recebe(char * buffer, char * quadro, int bytes);

 void copia_buffer_tx (char * buffer, int bytes);

 void copia_buffer_rx (char * buffer, int bytes);

 bool get_flag_tun();

 bool get_flag_serial();

 private:

  enum Estados {
   e0, e1
  };


  int _num_seq_tx;

  int _num_seq_rx;

  char  _dado_tx[256];

  char  _dado_rx[2*261];

  int _nBytes_tx;

  int _nBytes_rx;

  bool _flag_tun;

  bool _flag_serial;

  bool _ack;


   // estado atual da MEF
  Estados  _estado_atual;
  Enquadramento * arq_enquadra;
  tun * arq_tun;

};

#endif
