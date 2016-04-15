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
	this->porta = dev;
	_estado_atual = oscioso;
}

Enquadramento::~Enquadramento() {
}

void Enquadramento:: enviar(void * buffer, int bytes){

	int k=1;
	int i=0;
	int tam = (2*_max_bytes)+2;

	char quadro[tam];
	char * aux_buffer = (char *)buffer;

	if(bytes >= _min_bytes && bytes <= _max_bytes ){

		quadro [0] = '~';
		while(i < bytes){

			if(aux_buffer[i] != '~' && aux_buffer[i] != '}'){
				quadro[k] =  aux_buffer[i];
				i++;
				k++;
    			}
			if (aux_buffer[i] == '~' ){
				quadro[k]  = ']' xor 0x20;;
				quadro[k+1] = aux_buffer[i] xor 0x20;;
				i++;
				k = k + 2;
    			}
			if (aux_buffer[i] == '}'){
				quadro[k]  = ']' xor 0x20;;
				quadro[k+1] = aux_buffer[i] xor 0x20;;
				i++;
				k = k + 2;
     			}

 		}
 		quadro[k] = '~';
		k++;
		sleep(1);
		cout<<"msg: "<<quadro<<endl;
 		this->porta->Write(quadro, k);
	}else{
		cout<< "Erro mensagem com tamanho incorreto!"<<endl;
	}
}

int Enquadramento:: receber(void * buffer){

	bool c = false;
	int i = 0;
	char carac;
	char * aux_buffer = (char *)buffer;

	while(not c){
		this->porta->Read(aux_buffer+i, 1);
		carac = aux_buffer[i];
		c = mef_trata_byte(carac, aux_buffer);
		i++;

  	}
	buffer = (void *) aux_buffer;
	return n_bytes;
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
			estado_atual = escape;
			break;
		}
		if(byte!='~'){
			n_bytes = 0;
			buffer[n_bytes]= byte;
			n_bytes++;
			_estado_atual = recepcao;
		}else{
			_estado_atual = start;
			//cout << "msg: " << buffer<< endl;
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
				//cout << "msg: " << buffer<<" "<<n_bytes<< endl;
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
