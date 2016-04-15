/*
 * receptor.cpp
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

  Serial rf1("/dev/ttyUSB0", B9600);

  Enquadramento fra1(&rf1, 8,256);

  char buffer[256];

  sleep(1);
  int n = fra1.receber(buffer);

  cout << "Recebeu " << n << " bytes: ";

  for (int k=0; k < n; k++) cout << buffer[k];

  cout << endl;
}
