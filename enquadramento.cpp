#include "enquadramento.h"
#include "serial.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// 7E = '~';
// 7D = '}';
// 5E = '^';
// 5D = ']';

using namespace std;

Enquadramento::Enquadramento(Serial * dev, int byte_min, int byte_max){

	_min_bytes = byte_min;
	_max_bytes = byte_max;
	_numSeq =  0;
	this->porta = dev;
	_estado_atual = oscioso;
	this->n_bytes = 0;
}

Enquadramento::~Enquadramento() {
}

void Enquadramento:: enviar(char * buffer, int bytes, int tipMsg){
	
	crcInit();

	int k=1;
	int i=0;
	int tam = (2*_max_bytes)+8;
	short valu_crc;	
	char quadro[tam];
	char buffer_com_crc[bytes+4];


	buffer_com_crc[0] = tipMsg;
	cout<<"Tipo de mensagem: "<<(int)buffer_com_crc[0]<<endl;
	buffer_com_crc[1] =  get_numSeq();

	for (int k = 2; k < bytes+2; k++) {
		buffer_com_crc[k] = buffer[k-2];
	}
	
	valu_crc = crcFast(buffer_com_crc,(bytes+2));

	unsigned char high = (valu_crc >> 8);
	unsigned char low = (valu_crc & 0xff);
	buffer_com_crc[bytes + 2] = high;
	buffer_com_crc[bytes + 3] = low;

	//simula_erro(buffer_com_crc, (bytes+2));

		quadro [0] = '~';
		while(i < (bytes +4)){

			if(buffer_com_crc[i] != '~' && buffer_com_crc[i] != '}'){
				quadro[k] =  buffer_com_crc[i];
				i++;

				k++;
    			}
			if (buffer_com_crc[i] == '~' ){
				quadro[k]  = ']' xor 0x20;;
				quadro[k+1] = buffer_com_crc[i] xor 0x20;;
				i++;

				k = k + 2;
    			}
			if (buffer_com_crc[i] == '}'){
				quadro[k]  = ']' xor 0x20;;
				quadro[k+1] = buffer_com_crc[i] xor 0x20;;
				i++;

				k = k + 2;

     			}

 		}
 		quadro[k] = '~';
		k++;

		//cout<< "Mensagem enviada: "<<quadro<<endl;
 		this->porta->Write(quadro, k);
 		tcdrain(this->porta->get_serial());
}
int Enquadramento:: receber(char * buffer){

	crcInit();

	bool c = false;
	int i = 0;
	int n = 0;
	char carac;
	char head[2];
	char aux[_max_bytes+4];

	while(not c){
		int ret = this->porta->Read(buffer+i, 1, 5);
		 if (ret == 0){
			 cout<<"Timeout da serial"<<endl;
			 cout<<"Timeout da serial"<<endl;
			 cout<<"Timeout da serial"<<endl;
			 return 0;
		 }else{

			carac = buffer[i];
			c = mef_trata_byte(carac, buffer);
			i++;

		 }
	}

	unsigned char high = buffer[n_bytes-2];
	unsigned char low =  buffer[n_bytes-1];
	short crcTrans = low | (high << 8);

	short crcRec = crcFast(buffer,(n_bytes-2));

	
	if (crcTrans == crcRec) {
		
		head[0] = buffer[0];
		head[1] = buffer[1];
		
		cout<<"head[0]:  "<<head[0]<<endl;
		cout<<"head[1]:  "<<head[1]<<endl;

		n_bytes = n_bytes-2;
		cout<<"Mensagem recebida: "<<aux<<endl;
		return n_bytes;

	} else {
		cout << "Erro de CRC!" << endl;
		printf("crc transmissor: 0x%X\n", crcTrans);
		printf("crc receptor: 0x%X\n", crcRec);
		return 0;
	}

  }

  bool Enquadramento::mef_trata_byte(char byte, char * buffer){

  switch (_estado_atual){
	case oscioso:
		if(byte == '~'){
			n_bytes = 0;
			_estado_atual = start;
		}else{
			_estado_atual = oscioso;
		}
	break;

	case start:
		if(byte == '}' ){
			_estado_atual = escape;
			break;
		}
		if(byte!='~'){
			n_bytes = 0;
			buffer[n_bytes]= byte;
			n_bytes++;
			_estado_atual = recepcao;
		}else{
			_estado_atual = start;

		}
	break;

	case escape:
		if((byte == ']') ||(byte == '^')) {
			buffer[n_bytes] = byte xor 0x20;
			n_bytes++;
			_estado_atual = recepcao;
		}else{
			n_bytes = 0;
			cout << "erro de escape!"<<endl;
			_estado_atual = oscioso;
		}
	break;

	case recepcao:
		if(n_bytes > _max_bytes){
			cout<<"Erro de Overflow: "<<n_bytes<<" bytes"<<endl;
	  		n_bytes = 0;
			_estado_atual = oscioso;
			break;
		}else{

			if(byte == '}' ){
				_estado_atual = escape;
				break;
			}
			if(byte=='~'){
				_estado_atual = start;
				return true;
			}
	
			buffer[n_bytes]= byte;	
			n_bytes++;
			_estado_atual = recepcao;
			break;
		}
  }
  return false;
}

int Enquadramento::get_numSeq(){

	return _numSeq ;
}

void Enquadramento::set_numSeq(int num_seq){
	_numSeq = num_seq;
}

int Enquadramento::get_maxByte(){
	return _max_bytes;
}

int Enquadramento::get_minByte(){
	return _min_bytes;
}

unsigned long  Enquadramento::reflect( long data, char nBits) {
	long reflection = 0x00000000;
	char bit;

	for (bit = 0; bit < nBits; ++bit)
	{
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);

}


void Enquadramento::crcInit() {
	crc remainder;
	int dividend;
	char bit;

	for (dividend = 0; dividend < 256; ++dividend) {
		remainder = dividend << (8);
		for (bit = 8; bit > 0; --bit) {

			if (remainder & (1 << (15))) {
				remainder = (remainder << 1) ^ POLYNOMIAL;
			} else {
				remainder = (remainder << 1);
			}
		}

		crcTable[dividend] = remainder;
	}

}

crc Enquadramento::crcFast(char * message, int nBytes) {

	//cout<<"crc receptor dentro crc: "<<message<<"bytes: "<<nBytes<<endl;
	crc remainder = INITIAL_REMAINDER;
	unsigned char data;
	int byte;

	for (byte = 0; byte < nBytes; ++byte) {
		data = reflect(message[byte],8) ^ (remainder >> (8));
		remainder = crcTable[data] ^ (remainder << 8);
	}

	return (reflect(((remainder) ^ FINAL_XOR_VALUE), 16));

}

#define TAXA_ERRO 10 // 10%

void Enquadramento::simula_erro(char * f, int n) {
	static bool start = true;

	if (start) {
		start = false;
		srand(time(NULL));
	}

	bool erro = (rand() % 100) < TAXA_ERRO;

	if (erro) {
		int byte = rand() % n; // escolhe o byte
		f[byte] = f[byte] ^ 0xaa; // inverte alguns bits
		cout << "Simula erro: mudou byte " << byte << endl;
	}
}

