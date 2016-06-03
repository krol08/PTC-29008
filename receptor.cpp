/*
 * receptor.cpp
 *
 *  Created on: 15 de abr de 2016
 *      Author: root
 */

#include "serial.h"
#include "enquadramento.h"
#include "monitora.h"
#include "tun.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <cstdlib>
#include <sys/time.h>
#include <sys/types.h>

using namespace std;


int main() {

  Serial rf("/dev/ttyUSB0", B9600);
  Enquadramento fra(&rf, 8, 256);

// O que chega da tun, os 2 segundos bytes são ip ipv6
  char *dev = "interface_Ana&Karol";
  char * ip = "10.0.0.2";
  char * dst = "10.0.0.1";
  tun interface_tun (dev, ip, dst);


  Arq arq (&fra, &interface_tun);

  Monitora mon(&rf, &interface_tun, &fra, &arq);

  while(true){
 	cout << "Aguardando pacotes ..." << endl;
 	mon.sentinela();
  }

  return 0;


}
