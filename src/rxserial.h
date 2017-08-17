/*
 * rxserial.h
 *
 *  Created on: 2017/08/16
 *      Author: kouhei
 */

#ifndef RXSERIAL_H_
#define RXSERIAL_H_


#define BUFSIZE 1024

extern volatile unsigned char Buffer_protect;
extern volatile unsigned char SCI_stop;

void init_serial(void);
int writeBuffer(char *s);
int queryBufferSpace(void);
int sendString(char *s);
int strlen(char *s);
int itoa(int x, char *s);

#endif /* RXSERIAL_H_ */
