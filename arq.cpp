/*
 * Arq.cpp
 *
 *  Created on: 5 de mai de 2016
 *      Author: root
 */

#include "arq.h"
#include "enquadramento.h"
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

// Documentação:
// TIMEOUT = '0', ACK = '1', PAYLOAD = '2' e FRAME = '3';

Arq::Arq(Enquadramento * dev_enquadra, tun * dev_tun) {
	
	_num_seq_tx = 0;
	_num_seq_rx = 0;
	_estado_atual = e0;
	_nBytes = 0;
	_timeout = false;
	this->arq_enquadra = dev_enquadra;
	this->arq_tun = dev_tun;

}

Arq::~Arq() {
	// TODO Auto-generated destructor stub
}

void Arq::unpack(char * msg, int bytes) {

	printf("ARQ\n");
	for (int k = 0; k < bytes; k++) {
		printf("%x    ", msg[k]);
		if (k % 16 == 0) {
			printf("\n");
		}
	}
}
void Arq::mef(char * buffer, int bytes, tipo_ev even){

	char * quadro = buffer;
	//char  aux_buffer[(256*2)-4];
	char  buffer_ack [2];
	int num_bytes = 0;
	//static char buffer_tun[256];
	char * aux;
	int nSeq;


	switch (_estado_atual){
		case e0:
			if (even == PAYLOAD){

				cout<<"Estado: 0 -> Evento: PAYLOAD"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;
	         		// verificar tamanho!!!!!
				_estado_atual = e1;
				this->arq_enquadra->set_numSeq(_num_seq_tx);
				cout<<"Enviando quadro  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
				this->_dado = buffer;
				this->_nBytes = bytes;
				this->arq_enquadra->enviar(buffer, bytes,3);
				cout<<"TX-fim: "<<_num_seq_tx<<endl;
				cout<<"RX-fim: "<<_num_seq_rx<<endl;

			}

			if (even == FRAME){
				cout<<"Estado: 0 -> Evento: FRAME"<<(int)buffer[1]<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;

				this->arq_enquadra->set_numSeq(_num_seq_rx);
				nSeq = this->arq_enquadra->get_numSeq();

				if (buffer[1] == nSeq ){
					_estado_atual = e0;
					cout<<"Enviando para tun"<<endl;

 					unpack(buffer,bytes);


					char buffer_com_crc[bytes-2];
					int k;
					for ( k = 0; k < bytes-2; k++) {
						buffer_com_crc[k] = buffer[k+2];
	                                }

                                         unpack(buffer_com_crc,bytes-2);
					 cout<<"End: "<<endl;
					 int num = this->arq_tun->write(buffer_com_crc, k);
					// this->arq_enquadra->set_numSeq(_num_seq_rx);

					// Mandando ACK
					//this->arq_enquadra->set_numSeq(_num_seq_tx);
					cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;

					this->_dado = buffer;
					this->_nBytes = bytes;
					this->arq_enquadra->enviar(0, 0,1);
					_num_seq_rx = not _num_seq_rx;
                                        cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
					//this->arq_enquadra->set_numSeq(_num_seq_rx);

				        cout<<"TX-fim: "<<_num_seq_tx<<endl;
				         cout<<"RX-fim: "<<_num_seq_rx<<endl;

						}
				}

		break;

		case e1:
			
			if (even == TIMEOUT){
				// verificar tamanho!!!!!
	   			cout<<"Estado: 1 -> Evento: TIMEOUT"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;
				_estado_atual = e1;
				this->arq_enquadra->set_numSeq(_num_seq_tx);
				cout<<"Enviando quadro com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
				this->arq_enquadra->enviar(this->_dado, this->_nBytes,3);
			        cout<<"TX-fim: "<<_num_seq_tx<<endl;
				cout<<"RX-fim: "<<_num_seq_rx<<endl;

			}
			if (even == ACK){
				// verificar tamanho!!!!!
	   			cout<<"Estado: 1 -> Evento: NÃO ACK"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;
				this->arq_enquadra->set_numSeq(_num_seq_tx);
	   			nSeq = this->arq_enquadra->get_numSeq();
	   			if(buffer[1] != nSeq){
	   				_estado_atual = e1;
					_num_seq_tx = not _num_seq_tx;
	   				this->arq_enquadra->set_numSeq(_num_seq_tx);
					cout<<"Enviando quadro com sequência: "<<nSeq<<endl;
	   				this->arq_enquadra->enviar(this->_dado, this->_nBytes, 3);
					cout<<"TX-fim: "<<_num_seq_tx<<endl;
				        cout<<"RX-fim: "<<_num_seq_rx<<endl;
	    			}


			}
			if (even == FRAME){
	   			cout<<"Estado: 1 -> Evento: FRAME "<<"Nun seg"<<(int)buffer[1]<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;

	   			cout<<"Chegou Quadro com seq_"<<this->arq_enquadra->get_numSeq()<<endl;
				this->arq_enquadra->set_numSeq(_num_seq_rx);
	   			nSeq = this->arq_enquadra->get_numSeq();
	   			cout<<"Chegou Quadro depois do set com seq_"<<nSeq<<endl;

				if (buffer[1] == nSeq ){
					_estado_atual = e0;
					cout<<"Enviando para tun "<<endl;
					char buffer_com_crc[bytes-2];
					int k;
					for ( k = 0; k < bytes-2; k++) {
						buffer_com_crc[k] = buffer[k+2];
	                                  }

					int num = this->arq_tun->write(buffer_com_crc, k);
					unpack(buffer_com_crc,k);
					this->arq_enquadra->set_numSeq(_num_seq_rx);
					// Mandando ACK
					cout<<"Enviando ACK_"<<this->arq_enquadra->get_numSeq()<<endl;
					this->arq_enquadra->enviar(0, 0,1);

		   			cout<<"Invertando seq_"<<this->arq_enquadra->get_numSeq()<<endl;
					_num_seq_rx = not _num_seq_rx;

	                 cout<<"TX-fim: "<<_num_seq_tx<<endl;
				     cout<<"RX-fim: "<<_num_seq_rx<<endl;

				}
			}
			if (even == ACK){
				// verificar tamanho!!!!!
	   			cout<<"Estado: 1 -> Evento: ACK"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;

				this->arq_enquadra->set_numSeq(_num_seq_tx);
	   			nSeq = this->arq_enquadra->get_numSeq();
				if(buffer[1] == nSeq){
					cout<<"Aceito com sequência: "<<nSeq<<endl;
					_num_seq_tx = not _num_seq_tx;
                                         //this->arq_enquadra->set_numSeq(_num_seq_tx);
					_estado_atual = e0;

				}

				cout<<"TX-fim: "<<_num_seq_tx<<endl;
				cout<<"RX-fim: "<<_num_seq_rx<<endl;
			}
		break;
  	}
}

