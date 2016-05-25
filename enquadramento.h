#ifndef ENQUADRAMENTO_H
#define ENQUADRAMENTO_H

#include "serial.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


//n_bytes - tamanho do quadro, alimentado pela máquina de estados.
using namespace std;

	typedef unsigned short  crc;

	#define CRC_NAME		"CRC-16"
	#define POLYNOMIAL		0x8005
	#define INITIAL_REMAINDER	0x0000
	#define FINAL_XOR_VALUE		0x0000
	#define REFLECT_DATA		TRUE
	#define REFLECT_REMAINDER	TRUE
	#define CHECK_VALUE		0xBB3D

class Enquadramento {
 public:


  Enquadramento(Serial * dev, int bytes_min, int bytes_max);
  ~Enquadramento();

  // envia o quadro apontado por buffer
  // o tamanho do quadro é dado por bytes
  void enviar(char * buffer, int bytes, int tipMsg);

  // espera e recebe um quadro, armazenando-o em buffer
  // retorna o tamanho do quadro recebido
  int receber(char * buffer);

  int get_numSeq();

  void set_numSeq(int num_seq);

  int get_maxByte();

  int get_minByte();

  void  crcInit();
  crc   crcFast(char * message, int nBytes);
  unsigned long reflect( long data, char nBits);

  void simula_erro(char * f, int n);

  crc  crcTable[256];

 private:

  enum Estados {
    oscioso, start, recepcao, escape
  };

  int _min_bytes, _max_bytes;
  Serial * porta;

  // bytes recebidos pela MEF até o momento
  int n_bytes;
  int _numSeq;

  // estado atual da MEF
  Estados _estado_atual;

  // aqui se implementa a máquina de estados de recepção
  // retorna true se reconheceu um quadro completo
  // bytes de dados são gravados no buffer
  bool mef_trata_byte(char byte, char * buffer);

};

#endif
