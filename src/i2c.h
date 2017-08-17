/*
 * i2c.h
 *
 *  Created on: 2017/08/16
 *      Author: kouhei
 */

#ifndef I2C_H_
#define I2C_H_

#define I2C_WRITE 0x00
#define I2C_READ 0x01
#define GYRO 0xD2
#define GCTRL_REG1 0x20
#define GSTATUS_REG 0x27
#define GOUT_X_L 0x28
#define GOUT_X_H 0x29
#define GOUT_Y_L 0x2A
#define GOUT_Y_H 0x2B
#define GOUT_Z_L 0x2C
#define GOUT_Z_H 0x2D

void init_iic_master(void);
void iic_write_data(unsigned char address,unsigned char data[],int data_num);
void iic_read_data(unsigned char address, unsigned char data[],int data_num);

#endif /* I2C_H_ */
