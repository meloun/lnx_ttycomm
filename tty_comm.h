/* s2e.h - Serial To Ethernet header */

/* Copyright (c) 2008, Nissin Systems Co., Ltd. All rights reserved. */

/*
modification history
--------------------
01b,15jan10,nss  added SSH/SSL.
01a,05aug08,nss  written.
*/

#ifndef _TTY_COMM_H
#define _TTY_COMM_H

#include "tty.h"


/* COMMANDS */
// 0x00+ - spec command

#define CMD_SYNC_START         0x01      // Comm synchronization command
// synchtonization, posila se na konci jedne davky prenosu
// xport ma cas do dalsi davky -> muze si procistit buffery, resp. odpojit a pripojit seriovy port
#define CMD_SYNC_END           0x02


// 0x10+ - executive commands (set)
#define CMD_SET_OUTPUTS		0x10      // Set 16xoutputs

// 0x20+ -
#define CMD_GET_INPUTS  	0x20      // Get 16xiputs

//0x30+ - MESSMODUL state 1F commands(get)
#define CMD_MM_GET_FREQUENCE  	0x30      //
#define CMD_MM_GET_TEMPERATURE 	0x31      //


//0x40+ - MESSMODUL state 3F
#define CMD_MM_GET_VOLTAGE_1  	0x40      //
#define CMD_MM_GET_VOLTAGE_2  	0x41      //
#define CMD_MM_GET_VOLTAGE_3  	0x42      //

#define CMD_MM_GET_CURRENT_1  	0x43      //
#define CMD_MM_GET_CURRENT_2  	0x44      //
#define CMD_MM_GET_CURRENT_3  	0x45      //

#define CMD_MM_GET_POWER_1 		0x46      //
#define CMD_MM_GET_POWER_2 		0x47      //
#define CMD_MM_GET_POWER_3 		0x48      //

#define CMD_MM_GET_ENERGY_1  	0x49      //
#define CMD_MM_GET_ENERGY_2  	0x4A      //
#define CMD_MM_GET_ENERGY_3  	0x4B      //

#define CMD_MM_GET_PF_1  		0x4C      //
#define CMD_MM_GET_PF_2  		0x4D      //
#define CMD_MM_GET_PF_3  		0x4E      //


/*END OF COMMANDS*/


#define	TTY_BUF_SIZE	2048	/* size of tty buffer */

// protocol specification
#define COMM_XPORT_EMPTYFRAME_LENGTH      3         // empty frame length (datalength = 0)
#define COMM_XPORT_DATALENGTH_MAX		  30		  // maximal datalength, restricted by UART buffer max length
#define COMM_XPORT_STARTBYTE			  0x53      // Startbyte
#define COMM_XPORT_ACK_OFFSET             0x80      // Offset added to command tag = ACK

// protocol states
typedef enum {
	eWAIT_FOR_STARTBYTE, eWAIT_FOR_SEQ, eWAIT_FOR_CMD,  eWAIT_FOR_DATALENGTH, eWAIT_FOR_DATA, eWAIT_FOR_XOR, eWAIT_FOR_PROCESS
} eCOMM_XPORT_STATE;

typedef struct{
    volatile int comm_state; // current protocol state
    unsigned char seq;       // current sequence number
}tProtocol;

#define DATALENGHT	30
typedef struct{
	unsigned char command;
	unsigned char sequence;
	unsigned char data_cnt;		//delka dat(inkrementovana s prijimanymi daty)
	unsigned char datalength;	//delka dat(prijata)
	unsigned char data[DATALENGHT];
	unsigned char xor;
}tFRAME;



typedef struct {
	tTTY_CONF sConf;
	signed char buffer[TTY_BUF_SIZE];

	tProtocol sProtocol;
}tTTY_COMM;



#endif	/* _S2E_H */
