/*
 * Monitora.cpp
 *
 *  Created on: 09 de maio de 2016
 *      Author: Ana Luiza e Karoline
 */

#include "monitora.h"
#include "serial.h"
#include "tun.h"
#include "arq.h"
#include "enquadramento.h"
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

Monitora::Monitora(Serial * dev_serial, tun * dev_tun, Enquadramento * dev_enquadra, Arq * dev_arq) {

	this->mon_serial = dev_serial;
	this->mon_tun = dev_tun;
	this->mon_enquadra = dev_enquadra;
	this->mon_arq = dev_arq;
	this->flag_tun = true;
	this->flag_serial = true;
	this->ack = true;


}

Monitora::~Monitora() {
 
}

void Monitora::unpack(char * msg, int bytes) {


	printf("Monitora Unpank\n");
	for (int k = 0; k < bytes; k++) {
		printf("%x    ", msg[k]);
		if (k % 16 == 0) {
			printf("\n");
		}
	}
}

void Monitora::sentinela(){

	char buffer[256];
	int num = 0;
	int max_fd=0;
	struct timeval tv;
	int ret_select;

	if ((this->flag_tun == true) & (this->flag_serial == true)){
		enable();
	    cout<<"Enable"<<endl;
	}
	if (this->flag_tun == false){
		disable_tun();
		 cout<<"desabilita tun"<<endl;
	}
	if (this->flag_serial == false){
		disable_serial();
		 cout<<"desabilita serial"<<endl;

	}

	if (_fd_serial > _fd_tun){
		max_fd = _fd_serial;
	}else{
		max_fd = _fd_tun;
	}


	tv.tv_sec = 0;
    tv.tv_usec = 600000;

	ret_select = select(max_fd+1, &_readset, NULL, NULL, &tv);

	
	if (ret_select == -1){
		cout<< "Erro: "<<ret_select<<endl;
	}else{

		if (ret_select == 0){
			cout<< " tun: "<<this->flag_tun<<endl;
			if(this->flag_tun == false){
				this->mon_arq->mef(0, 0, Arq::TIMEOUT);
				cout<< "Timeout ACK! "<<endl;

			}
			cout<< "Timeout aquii!! "<<endl;
		}else{
			if(FD_ISSET(_fd_serial,&_readset)){
				cout<< "ESTOU NA SERIAL "<<endl;


				int num_bytes = this->mon_enquadra->receber(buffer);
			    unpack(buffer,num_bytes);
			    if (num_bytes > 0){
			    	if (buffer[0] == 1){
			    		this->flag_serial = true;
			    		this->flag_tun= true;
			    		this->ack = true;
			    		this->mon_arq->mef(buffer, num_bytes, Arq::ACK);

			    	}else{
						this->flag_serial = true;
						if(this->ack == true){
							this->flag_tun= true;
							cout<< "estou ack true"<<endl;

						}else{
							this->flag_tun= false;
							cout<< "estou ack false"<<endl;
						}
							this->mon_arq->mef(buffer, num_bytes, Arq::FRAME);
					}
			    }

			}

			if(FD_ISSET(_fd_tun,&_readset)){
				cout<< "ESTOU NA TUN "<<endl;
				num = this->mon_tun->read(buffer, 256);
				unpack(buffer,num);
				this->flag_serial = true;
				this->flag_tun= false;
				this->ack = false;
				this->mon_arq->mef(buffer,num,Arq::PAYLOAD);
			}
		}			
	}
	
}
void Monitora::enable(){
	FD_ZERO(&_readset);

	_fd_serial = this->mon_serial->get_serial();
	_fd_tun = this->mon_tun->get_tun();

	FD_SET(_fd_serial,&_readset);
	FD_SET(_fd_tun,&_readset);
}

void Monitora::disable_tun(){
	FD_ZERO(&_readset);
	_fd_serial = this->mon_serial->get_serial();
	_fd_tun = 0;

	FD_SET(_fd_serial,&_readset);

}

void Monitora::disable_serial(){
	FD_ZERO(&_readset);
	_fd_serial = 0;
	_fd_tun = this->mon_tun->get_tun();

	FD_SET(_fd_tun,&_readset);
}
