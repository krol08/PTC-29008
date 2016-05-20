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
	fd_set readset;

	FD_ZERO(&readset);

	int fd_serial = this->mon_serial->get_serial();
	int fd_tun = this->mon_tun->get_tun();

	if (fd_serial > fd_tun){
		max_fd = fd_serial;
	}else{
		max_fd = fd_tun;
	}

	FD_SET(fd_serial,&readset);
	FD_SET(fd_tun,&readset);
	tv.tv_sec = 4;
    	tv.tv_usec = 0;

	ret_select = select(max_fd+1, &readset, NULL, NULL, &tv);

	
	if (ret_select == -1){
		cout<< "Erro: "<<ret_select<<endl;
	}else{

		if (ret_select == 0){
			cout<< "Timeout!! "<<endl;
		}else{
			if(FD_ISSET(fd_serial,&readset)){
				cout<< "ESTOU NA SERIAL "<<endl;


				int num_bytes = this->mon_enquadra->receber(buffer);
			    unpack(buffer,num_bytes);
				if (buffer[0] == 1){
					this->mon_arq->mef(buffer, num_bytes, Arq::ACK);
				}else{

					this->mon_arq->mef(buffer, num_bytes, Arq::FRAME);
				}


			}

			if(FD_ISSET(fd_tun,&readset)){
				cout<< "ESTOU NA TUN "<<endl;
				num = this->mon_tun->read(buffer, 256);
				unpack(buffer,num);
				this->mon_arq->mef(buffer,num,Arq::PAYLOAD);
			}
		}			
	}
	
}
