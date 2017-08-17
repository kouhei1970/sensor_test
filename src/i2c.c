/*
 * i2c.c
 *
 *  Created on: 2017/08/16
 *      Author: kouhei
 */
#include "iodefine.h"
#include "i2c.h"

//Master側iicイニシャライズ
void init_iic_master(void){

	SYSTEM.MSTPCRB.BIT.MSTPB21 = 0; //I2Cモジュールスタンバイ解除
	MSTP(RIIC0) = 0;

	RIIC0.ICCR1.BIT.ICE = 0;		//初期設定中はI2Cを停止
	RIIC0.ICCR1.BIT.IICRST = 1;		//I2Cリセット
	RIIC0.ICCR1.BIT.IICRST = 0;		//I2Cリセット解除

//	RIIC0.ICSER.BIT.SAR0E = 1;		//SARL0,SARU0の設定値有効
//	RIIC0.ICSER.BIT.SAR1E = 1;		//SARL1,SARU1の設定値有効
	RIIC0.ICSER.BIT.GCAE = 0;		//ジェネラルコードアドレス無効

	////////////////////////////
	// IIC phi = PCLK/2 clock //
	// 400kbps (372k)(PCLK = 24MHz) //
	////////////////////////////
	RIIC0.ICMR1.BIT.CKS = 1;
	RIIC0.ICBRH.BIT.BRH = 10;//7
	RIIC0.ICBRL.BIT.BRL = 20;//16 //31

	RIIC0.ICMR2.BYTE = 0x00;		//タイムアウト検出の条件設定 使う場合はコメント
	RIIC0.ICMR3.BYTE = 0x10;
	RIIC0.ICMR3.BIT.ACKWP = 1;		//NACk/ACK信号の送信選択許可(RIIC0.ICMR3.BIT.ACKBTへの書き込み許可)

	/* iic用ピン設定 */
	PORT1.DDR.BIT.B2 = 0;	//i2c:SCL0の使用
	PORT1.DDR.BIT.B3 = 0;	//i2c:SDA0の使用
	PORT1.ICR.BIT.B2 = 1;	//i2c:SCL 入力バッファ有効
	PORT1.ICR.BIT.B3 = 1;	//i2c:SDA 入力バッファ有効

	RIIC0.ICFER.BIT.NFE = 1;		//デジタルノイズフィルタ使用

	RIIC0.ICCR1.BIT.ICE = 1;		//I2C転送動作可能状態
}

/***********************************************************
 * 　addressにある機器にdata[]の中にあるデータを書き込む
 **********************************************************/
 //master 送信関数
void iic_write_data(unsigned char address,unsigned char data[],int data_num)
{
	int i;

	while(RIIC0.ICCR2.BIT.BBSY);			//BBSY=1でI2Cパス占有状態
	RIIC0.ICCR2.BIT.ST = 1;  				//開始条件発行

	if(RIIC0.ICSR2.BIT.NACKF==0){			//NACK未検出
		while(!RIIC0.ICSR2.BIT.TDRE);		//ICDRTレジスタに送信データなし
		RIIC0.ICDRT = address | I2C_WRITE; 	//=(16進数)0xAA = (2進数)10101010
											//slaveアドレス7ビット+「write」1ビット

		for(i=0;i<data_num;i++){
			while(!RIIC0.ICSR2.BIT.TDRE);	//ICDRTレジスタに送信データなし
			RIIC0.ICDRT = data[i];			//データライト
		}
		while(!RIIC0.ICSR2.BIT.TEND);		//データ送信終了
	}

	RIIC0.ICSR2.BIT.STOP = 0;				//ストップコンディション未検出
	RIIC0.ICCR2.BIT.SP = 1;					//停止条件発行

	while(!RIIC0.ICSR2.BIT.STOP);			//停止条件発行確認 (1になるまで待機)

	//次の通信のための処理
	RIIC0.ICSR2.BIT.NACKF = 0;				//NACK未検出
	RIIC0.ICSR2.BIT.STOP = 0;				//ストップコンディション未検出

}

/***********************************************************
 * addressにある機器からdata[]へ、データを読み込む。
 * この実行前に iic_write_data()によって、デバイスのregister map
 * を送信しておく必要あり。
 **********************************************************/
 //master 受信関数
void iic_read_data(unsigned char address, unsigned char data[],int data_num)
{
	int i;
	unsigned char dummy;

	while(RIIC0.ICCR2.BIT.BBSY);		//BBSY=1でI2Cパス占有状態

	//まず,マスター送信モードでアドレスとREADを書き込み
	RIIC0.ICCR2.BIT.ST = 1;  		//開始条件発行
	while(!RIIC0.ICSR2.BIT.TDRE);		//ICDRTレジスタに送信データなし
	RIIC0.ICDRT = address | I2C_READ; 	//データライト(アドレス)

	//マスタ受信モード
	while(!RIIC0.ICSR2.BIT.RDRF);		//ICDRRレジスタに受信データあり

	if(RIIC0.ICSR2.BIT.NACKF==0){		//NACK未検出
		dummy = RIIC0.ICDRR;		//ダミーリード：これが受信のきっかけ

		for(i=0;i<data_num-1;i++){
			while(!RIIC0.ICSR2.BIT.RDRF);	//ICDRRレジスタに受信データあり
			if(i==data_num-2){			//最後-1のデータ
				RIIC0.ICMR3.BIT.ACKBT = 1;	//データ受信完了(スレーブにNACK送信)
			}
			if(i==data_num-3){			//最後-2のデータ
				RIIC0.ICMR3.BIT.WAIT = 1;	//Lowにホールド
			}
			data[i] = RIIC0.ICDRR;			//受信したデータ格納
		}

		while(!RIIC0.ICSR2.BIT.RDRF);	//ICDRRレジスタに受信データあり
		RIIC0.ICSR2.BIT.STOP = 0;		//ストップコンディション未検出
		RIIC0.ICCR2.BIT.SP = 1;			//停止条件発行
		data[data_num-1] = RIIC0.ICDRR;	//受信した最後のデータ格納
		RIIC0.ICMR3.BIT.WAIT = 0;		//Lowにホールド解除
	}
	else{								//NACK検出
		RIIC0.ICSR2.BIT.STOP = 0;		//ストップコンディション未検出
		RIIC0.ICCR2.BIT.SP = 1;			//停止条件発行
		dummy = RIIC0.ICDRR;			//受信レジスタのダミーリード
	}

	while(!RIIC0.ICSR2.BIT.STOP);		//停止条件発行確認<-------バグの場所！！！！！

	//次の通信のための処理
	RIIC0.ICSR2.BIT.NACKF=0;	//NACK未検出
	RIIC0.ICSR2.BIT.STOP=0;		//ストップコンディション未検出

}
