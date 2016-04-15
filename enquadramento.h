#ifndef ENQUADRAMENTO_H
#define ENQUADRAMENTO

#include "serial.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;

class Enquadramento {
 public:

  Enquadramento(Serial * dev, int bytes_min, int bytes_max);
  ~Enquadramento();

  // envia o quadro apontado por buffer
  // o tamanho do quadro é dado por bytes
  void enviar(void * buffer, int bytes);

  // espera e recebe um quadro, armazenando-o em buffer
  // retorna o tamanho do quadro recebido
  int receber(void * buffer);

 private:

  enum Estados {
    oscioso, start, recepcao, escape
  };

  int _min_bytes, _max_bytes;
  Serial * porta;

  //n_bytes - tamanho do quadro, alimentado pela máquina de estados. 
  // bytes recebidos pela MEF até o momento
  int n_bytes;

  // estado atual da MEF
  Estados _estado_atual;

  // aqui se implementa a máquina de estados de recepção
  // retorna true se reconheceu um quadro completo
  // bytes de dados são gravados no buffer
  bool mef_trata_byte(char byte, char * buffer);

};

#endif
