/*
 * transmissor.cpp
 *
 *  Created on: 15 de abr de 2016
 *      Author: Karoline da Rocha e Ana Luiza Scharf
 */

#include "serial.h"
#include "enquadramento.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;

int main() {
  Serial rf("/dev/ttyUSB0", B9600);

  Enquadramento fra(&rf, 8,256);


 char * msg = "Um teste~~] do trans~m}i]ssor via ~serial!!!";
 //char * msg = "Um teste~~] d"; 
 char buffer[256];

 // int n = rf.Write(msg, strlen(msg));
  fra.enviar(msg,strlen(msg));
  //cout << "Enviou " << n << " bytes" << endl;
}
