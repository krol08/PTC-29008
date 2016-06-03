/*
 * Arq.cpp
 *
 *  Created on: 5 de mai de 2016
 *      Author: Karoline e Ana Luiza
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
	
	this->_num_seq_tx = 0;
	this->_num_seq_rx = 0;
	this->_estado_atual = e0;
	this->_nBytes_tx = 0;
	this->_nBytes_rx = 0;
	this->_backoff = 0;
	this->_flag_tun = true;
	this->_flag_serial= true;
	this->_ack = true;
	this->arq_enquadra = dev_enquadra;
	this->arq_tun = dev_tun;
	this->_timerBackoff = false;

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

	int tam = this->arq_enquadra->get_maxByte();
	char quadro[tam];
	int num_bytes = 0;
	int num = 0;
	int nSeq;
	char buffer_aux[bytes-2];
	int k = 0;

	switch (_estado_atual){
			case e0:

				if (even == PAYLOAD){
					if(veriTam(bytes) == true){
						cout<<"Estado: 0 -> Evento: PAYLOAD"<<endl;
						cout<<"TX: "<<_num_seq_tx<<endl;
						cout<<"RX: "<<_num_seq_rx<<endl;

						this->_flag_serial = true;
						this->_flag_tun = false;
						this->_ack = false;

						_estado_atual = e1;

						num_bytes = tipEther_envia(buffer, quadro,bytes);
						unpack(quadro,num_bytes);

						this->arq_enquadra->set_numSeq(_num_seq_tx);
						cout<<"Enviando quadro  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;

						copia_buffer_tx (quadro,num_bytes);
						unpack(_dado_tx,_nBytes_tx);

						this->arq_enquadra->enviar(quadro, num_bytes,3);
						cout<<"TX-fim: "<<_num_seq_tx<<endl;
						cout<<"RX-fim: "<<_num_seq_rx<<endl;
					}
				}

				if (even == FRAME){
					if(veriTam(bytes) == true){
						cout<<"Estado: 0 -> Evento: FRAME"<<(int)buffer[1]<<endl;
						cout<<"TX: "<<_num_seq_tx<<endl;
						cout<<"RX: "<<_num_seq_rx<<endl;

						this->_flag_serial = true;
						this->_flag_tun = true;

						this->arq_enquadra->set_numSeq(_num_seq_rx);
						nSeq = this->arq_enquadra->get_numSeq();


						if (buffer[1] == nSeq ){
							_estado_atual = e0;
							cout<<"Enviando para tun"<<endl;

							for ( k = 0; k < bytes-2; k++) {
								buffer_aux[k] = buffer[k+2];
							}

							num_bytes = tipEther_recebe(buffer_aux, quadro,k);
							num = this->arq_tun->write(quadro, num_bytes);

							// Mandando ACK
							cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;

							copia_buffer_rx (buffer,bytes);

							this->arq_enquadra->enviar(0, 0,1);
							_num_seq_rx = not _num_seq_rx;
							cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
							cout<<"TX-fim: "<<_num_seq_tx<<endl;
							cout<<"RX-fim: "<<_num_seq_rx<<endl;

						}
					}
				 }
			break;

			case e1:

				if (even == FRAME){
					if(veriTam(bytes) == true){
					cout<<"Estado: 1 -> Evento: FRAME"<<(int)buffer[1]<<endl;
					cout<<"TX: "<<_num_seq_tx<<endl;
					cout<<"RX: "<<_num_seq_rx<<endl;

					this->_flag_serial = true;
					this->_flag_tun = false;

					this->arq_enquadra->set_numSeq(_num_seq_rx);
					nSeq = this->arq_enquadra->get_numSeq();


					if (buffer[1] == nSeq ){
						_estado_atual = e1;
						cout<<"Enviando para tun"<<endl;

						for ( k = 0; k < bytes-2; k++) {
							buffer_aux[k] = buffer[k+2];
						}

						num_bytes = tipEther_recebe(buffer_aux, quadro,k);
						num = this->arq_tun->write(quadro, num_bytes);

						// Mandando ACK
						cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;

						copia_buffer_rx (buffer,bytes);

						this->arq_enquadra->enviar(0, 0,1);
						_num_seq_rx = not _num_seq_rx;
						cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
						cout<<"TX-fim: "<<_num_seq_tx<<endl;
						cout<<"RX-fim: "<<_num_seq_rx<<endl;
					}
				}
			}

			if (even == TIMEOUT){
				cout<<"Estado: 1 -> Evento: TIMEOUT"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;

				set_backoff();
				_estado_atual = e3;

			}

			if (even == ACK){
				cout<<"Estado: 1 -> Evento: NÃO ACK"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;

				this->arq_enquadra->set_numSeq(_num_seq_tx);
				nSeq = this->arq_enquadra->get_numSeq();
				if(buffer[1] != nSeq){
					_estado_atual = e3;

					this->_flag_serial = true;
					this->_flag_tun = false;

					set_backoff();
					_estado_atual = e3;
				}
			}

			if (even == ACK){
		   		cout<<"Estado: 1 -> Evento: ACK"<<endl;
				cout<<"TX: "<<_num_seq_tx<<endl;
				cout<<"RX: "<<_num_seq_rx<<endl;

				this->arq_enquadra->set_numSeq(_num_seq_tx);
		   		nSeq = this->arq_enquadra->get_numSeq();
				if(buffer[1] == nSeq){
					cout<<"Aceito com sequência: "<<nSeq<<endl;
					_num_seq_tx = not _num_seq_tx;

					 set_backoff();
					_estado_atual = e2;


				}

				cout<<"TX-fim: "<<_num_seq_tx<<endl;
				cout<<"RX-fim: "<<_num_seq_rx<<endl;
			}

			break;

			case e2:

				if (even == FRAME){
					if(veriTam(bytes) == true){
						cout<<"Estado: 2 -> Evento: FRAME"<<(int)buffer[1]<<endl;
						cout<<"TX: "<<_num_seq_tx<<endl;
						cout<<"RX: "<<_num_seq_rx<<endl;

						this->_flag_serial = true;
						this->_flag_tun = false;

						this->arq_enquadra->set_numSeq(_num_seq_rx);
						nSeq = this->arq_enquadra->get_numSeq();


						if (buffer[1] == nSeq ){
							_estado_atual = e2;
							cout<<"Enviando para tun"<<endl;

							for ( k = 0; k < bytes-2; k++) {
								buffer_aux[k] = buffer[k+2];
							}

							num_bytes = tipEther_recebe(buffer_aux, quadro,k);
							num = this->arq_tun->write(quadro, num_bytes);

							// Mandando ACK
							cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;

							copia_buffer_rx (buffer,bytes);

							this->arq_enquadra->enviar(0, 0,1);
							_num_seq_rx = not _num_seq_rx;
							cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
							cout<<"TX-fim: "<<_num_seq_tx<<endl;
							cout<<"RX-fim: "<<_num_seq_rx<<endl;
						}
					}
				}

				if (even == BACKOFF){

					this->_flag_serial = true;
					this->_flag_tun = true;
					this->_timerBackoff = false;
					_estado_atual = e0;

				}

			break;

			case e3:

				if (even == FRAME){
					if(veriTam(bytes) == true){
						cout<<"Estado: 3 -> Evento: FRAME"<<(int)buffer[1]<<endl;
						cout<<"TX: "<<_num_seq_tx<<endl;
						cout<<"RX: "<<_num_seq_rx<<endl;

						this->_flag_serial = true;
						this->_flag_tun = false;

						this->arq_enquadra->set_numSeq(_num_seq_rx);
						nSeq = this->arq_enquadra->get_numSeq();


						if (buffer[1] == nSeq ){
							_estado_atual = e3;
							cout<<"Enviando para tun"<<endl;

							for ( k = 0; k < bytes-2; k++) {
								buffer_aux[k] = buffer[k+2];
							}

							num_bytes = tipEther_recebe(buffer_aux, quadro,k);
							num = this->arq_tun->write(quadro, num_bytes);

							// Mandando ACK
							cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;

							copia_buffer_rx (buffer,bytes);

							this->arq_enquadra->enviar(0, 0,1);
							_num_seq_rx = not _num_seq_rx;
							cout<<"Enviando ACK  com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
							cout<<"TX-fim: "<<_num_seq_tx<<endl;
							cout<<"RX-fim: "<<_num_seq_rx<<endl;
						}
					}
				}

				if (even == BACKOFF){

					this->_flag_serial = true;
					this->_flag_tun = false;
					this->_timerBackoff = false;
					_estado_atual = e1;

					this->arq_enquadra->set_numSeq(_num_seq_tx);
					unpack(this->_dado_tx,this->_nBytes_tx);

					cout<<"Enviando quadro com sequência: "<<this->arq_enquadra->get_numSeq()<<endl;
					this->arq_enquadra->enviar(this->_dado_tx, this->_nBytes_tx,3);
					cout<<"TX-fim: "<<_num_seq_tx<<endl;
					cout<<"RX-fim: "<<_num_seq_rx<<endl;
				}
			break;
	}

}

bool Arq::veriTam(int bytes){

	int max = this->arq_enquadra->get_maxByte();
	int min = this->arq_enquadra->get_minByte();

	if ((bytes >= min) & (bytes <= max)){
		//cout<<"Tamanho da mensagem correto"<<endl;
		return true;

	}else{
		cout<<"Tamanho da mensagem incorreto"<<endl;
		return false;
	}
}

int Arq::tipEther_envia(char * buffer, char * quadro, int bytes){

	//char aux [3];
	//char * ipv4 = "0080";
	//char * ipv6 = "DD68";
	int k =0;

	//for(int i = 0; i < 4; i++){
		//aux[i] = buffer [i];

	//}

	 //if (aux == ipv4){
		//quadro[0] = 1;
		//cout<<"Sou ipv4"<<endl;
	 //}else{
		// if(aux == ipv6){
			// quadro[0] = 2;
			//cout<<"Sou ipv6"<<endl;
		 //}else{
			// quadro[0] = 0;
			//cout<<"Sou os demais "<<ipv4<<"   "<<ipv6<<"  "<<aux<<endl;

		 //}
	 //}

	// Tipo IPV4:
	 if ((buffer[0] == 0)&(buffer[1] == 0)&(buffer[2] == 8)&(buffer[3] == 0)){
		 quadro[0] = 1;
	 }else{
		 // Tipo IPV6:
		 if((buffer[0] == 'D')&(buffer[1] == 'D')&(buffer[2] == 6)&(buffer[3] == 8)){
			 quadro[0] = 2;
		 }else{
			 // Outros tipos Ethernet:
			 quadro[0] = 0;
		 }

	 }
	 for(k = 1; k < bytes-3; k++){
		 quadro[k] = buffer[k+3];
	 }

	 return k;
}

int Arq::tipEther_recebe(char * buffer, char * quadro, int bytes){


	int k =0;

	// Tipo IPV4:
	if (buffer[0] == 1){
		quadro[0] = 0;
		quadro[1] = 0;
		quadro[2] = 8;
		quadro[3] = 0;

	}else{
		// Tipo IPV6:
		if (buffer[0] == 2){
				quadro[0] = 'D';
				quadro[1] = 'D';
				quadro[2] = '6';
				quadro[3] = '8';

		}else{
			// Outros tipos Ethernet:
			quadro[0] = 0;
			quadro[1] = 0;
			quadro[2] = 0;
			quadro[3] = 0;
		}
	}

	 for(k = 4; k < bytes+3; k++){
		 quadro[k] = buffer[k-3];
	 }
	 return k;
}

void Arq::copia_buffer_tx (char * buffer, int bytes){

	for(int k = 0; k < bytes; k++){
		this->_dado_tx[k] = buffer[k];
	}
	this->_nBytes_tx = bytes;
}

void Arq::copia_buffer_rx (char * buffer, int bytes){

	for(int k = 0; k < bytes; k++){
		this->_dado_rx[k] = buffer[k];
	}
	this->_nBytes_rx = bytes;
}

bool Arq::get_flag_serial(){

	return this->_flag_serial;
}

bool Arq::get_flag_tun(){

	return this->_flag_tun;
}

void Arq::set_backoff(){

	this->_timerBackoff = true;
	srand(time(NULL));
	this->_backoff = rand() % 200;

}

int Arq::get_backoff(){
	return (this->_backoff*1000);
}

bool Arq::get_TimerBackoff(){
	return this->_timerBackoff;
}
