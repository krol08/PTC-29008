/*
 * tun.h
 *
 *  Created on: 03/05/2016
 *      Author: Karoline e Ana Luiza
 */

#ifndef TUN_H_
#define TUN_H_
using namespace std;

class tun {
protected:
	int tty_fd_tun;
public:
	tun(char *dev, char * ip, char * dst);
	virtual ~tun();


	int tun_alloc(char *dev);
	int set_ip(char *dev, char * ip, char * dst);
	// envia size bytes contidos em buffer
	// retorna a quantidade de bytes de fato enviados
	int write(char * buffer, int size);

	// lê até size bytes, e os armazena em buffer
	// retorna a quantidade de bytes de fato recebidos
	int read(char * buffer, int size);
	int get_tun();
};

#endif /* TUN_H_ */
