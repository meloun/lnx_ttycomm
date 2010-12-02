/***********************************
 * tty_comm, ing.Lubos Melichar
 ***********************************
 * - received chars throught uart
 *   are written to console
 **********************************/

#include <stdio.h>
#include "tty_comm.h"
#include "../my_libc/io_common.h"
#include "../my_libc/pm_common.h"

//
tProtocol sProtocol;
tFRAME	sFrame;
tTTY_CONF sConf;
signed char buffer[TTY_BUF_SIZE];

void tty_comm_handler(unsigned char data);
unsigned char tty_comm_process_command(void);



void tty_comm_init(){}

//toDo: hodnoty pres definy z tty_comm.h
char tty_comm_connect(){

    //printf("I: start.. \n");

    memset (&sConf, 0, sizeof (sConf));

    //set configaration struct
    strcpy (sConf.tty_device, "/dev/ttyS0");
	sConf.tty_baudrate = 9600;
	sConf.tty_parity = 0;
	sConf.tty_stopbit = 1;
	sConf.tty_length = 8;
	sConf.tty_flow = 0;
	sConf.tty_fd = -1;
	//sConf.tty_buffer = buffer;
	//sConf.tty_bufsz = sizeof (buffer);

	tty_open(&sConf);

}


void tty_comm_disconnect(){
	tty_close(&sConf);

}

void tty_comm_manager(){
	unsigned char aux_data;

	//new char to handler
	if( tty_read(&sConf, &aux_data, 1) !=- 1 ) //new char?
		tty_comm_handler(aux_data);

	//process succesfully received frame
    if (sProtocol.comm_state == eWAIT_FOR_PROCESS) {     // new frame?

        // Executing received command and new frame creation
    	tty_comm_process_command();   // command executed ok
    	sProtocol.comm_state = eWAIT_FOR_STARTBYTE;
    }
	fflush(stdout);

}

void tty_comm_handler(unsigned char data){
	switch(sProtocol.comm_state){
		case eWAIT_FOR_STARTBYTE:
			if (data == COMM_XPORT_STARTBYTE) {    // start of frame
				sFrame.data_cnt = 0;          // init databytes counter
				sFrame.datalength = 0;
				//printf("\nI: tty: prijat start byte");
				sProtocol.comm_state = eWAIT_FOR_SEQ;
			}
			break;
		case eWAIT_FOR_SEQ:                  // awaiting sequence byte
			sFrame.sequence = data;            // seq
			sProtocol.comm_state = eWAIT_FOR_CMD;  // switch to nex state
			break;

		case eWAIT_FOR_CMD:                         // awaiting command byte
			sFrame.command = data;               // command
			sProtocol.comm_state = eWAIT_FOR_DATALENGTH;  // switch to next state
			break;

		case eWAIT_FOR_DATALENGTH:  // awaiting datalength byte
			if(data > COMM_XPORT_DATALENGTH_MAX){        // datalength above data buffer (ERROR)
				printf("\nE: prilis velka delka dat: %d, %x", data, sFrame.command );
				sProtocol.comm_state = eWAIT_FOR_STARTBYTE; // switch to starting state
			}
			else {      // ok
				sFrame.datalength = data;
				if (data)     // frame contains data
					sProtocol.comm_state = eWAIT_FOR_DATA;        // switch to next state
				else          // no data, xor is expected
					sProtocol.comm_state = eWAIT_FOR_XOR;			// switch to xor state
			}
			break;

		case eWAIT_FOR_DATA:              // awaiting data byte
			sFrame.data[sFrame.data_cnt] = data;  // data byte
			sFrame.data_cnt++;                       // saved data cntr increases
			if (sFrame.data_cnt >= sFrame.datalength){ // last data
				sProtocol.comm_state = eWAIT_FOR_XOR;       // switch to next (xor) state
			}
		// 			uartSendByte(0,sFrame_Rx.data_cnt);
		// 			LED_2_CHANGE;
			break;

		case eWAIT_FOR_XOR: // awaiting xor
			sFrame.xor = data;               // xor
			sProtocol.comm_state = eWAIT_FOR_PROCESS; // switch to final state - awating processing
			break;
	} // switch end

}

// CommApp_ProcessCommand - command executing and filling
// Tx Frame with valid data and datalength. Return 1 when
// gets unknown command, else return 0 (OK).
unsigned char tty_comm_process_command(void){
	tPM aux_sPm;
	tIO aux_sIo;
	unsigned char index;


	//read from file
	if(sFrame.command < (0x0F | 0x80))   	//system command?
		;
	else if(sFrame.command < (0x1F | 0x80))  //IO command?
		io_read_config (&aux_sIo);
	else if(sFrame.command < (0x4F | 0x80)){  //Messmodul command?
		pm_read_config (&aux_sPm);
		//printf("\nread current: %d, %d, %d", aux_sPm.sLines[0].current, aux_sPm.sLines[1].current,aux_sPm.sLines[2].current );
	}

	//printf("CMD:%x",sFrame.command);
	switch(sFrame.command){
		case (CMD_GET_INPUTS | 0x80):
			aux_sIo.ext_inputs[0].current_state = sFrame.data[0] >> 1;
			aux_sIo.ext_inputs[1].current_state = sFrame.data[0] & 0x01;

			printf("\n CMD_GET_INPUTS: %d, %d", aux_sIo.ext_inputs[0].current_state, aux_sIo.ext_inputs[1].current_state);
			break;
		case (CMD_SET_OUTPUTS | 0x80):

			printf("\n CMD_SET_OUTPUTS: ");
			break;
		case (CMD_MM_GET_FREQUENCE | 0x80):

			aux_sPm.frequence = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_FREQUENCE: %u", aux_sPm.frequence);
			break;

		case (CMD_MM_GET_TEMPERATURE | 0x80):

			aux_sPm.temperature = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_TEMPERATURE: %d", aux_sPm.temperature);
			break;

		case (CMD_MM_GET_VOLTAGE_1 | 0x80):
		case (CMD_MM_GET_VOLTAGE_2 | 0x80):
		case (CMD_MM_GET_VOLTAGE_3 | 0x80):

			index = sFrame.command - (CMD_MM_GET_VOLTAGE_1 | 0x80);
			aux_sPm.sLines[index].voltage = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_VOLTAGE_%d: %u", index, aux_sPm.sLines[index].voltage);
			break;

		case (CMD_MM_GET_CURRENT_1 | 0x80):
		case (CMD_MM_GET_CURRENT_2 | 0x80):
		case (CMD_MM_GET_CURRENT_3 | 0x80):

			index = sFrame.command - (CMD_MM_GET_CURRENT_1 | 0x80);
			aux_sPm.sLines[index].current = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_CURRENT_%d: %d", index, aux_sPm.sLines[index].current);
			break;

		case (CMD_MM_GET_POWER_1 | 0x80):
		case (CMD_MM_GET_POWER_2 | 0x80):
		case (CMD_MM_GET_POWER_3 | 0x80):

			index = sFrame.command - (CMD_MM_GET_POWER_1 | 0x80);
			aux_sPm.sLines[index].power = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_POWER_%d: %u", index, aux_sPm.sLines[index].power);
			break;

		case (CMD_MM_GET_ENERGY_1 | 0x80):
		case (CMD_MM_GET_ENERGY_2 | 0x80):
		case (CMD_MM_GET_ENERGY_3 | 0x80):

			index = sFrame.command - (CMD_MM_GET_ENERGY_1 | 0x80);
			aux_sPm.sLines[index].energy = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_ENERGY_%d: %u", index, aux_sPm.sLines[index].energy);
			break;

		case (CMD_MM_GET_PF_1 | 0x80):
		case (CMD_MM_GET_PF_2 | 0x80):
		case (CMD_MM_GET_PF_3 | 0x80):

			index = sFrame.command - (CMD_MM_GET_PF_1 | 0x80);
			aux_sPm.sLines[index].pf = sFrame.data[0] + (sFrame.data[1]<<8);

			printf("\n CMD_MM_GET_PF_%d: %u", index, aux_sPm.sLines[index].pf);
			break;

		case (CMD_SYNC_END | 0x80):

	        tty_comm_disconnect();
	        tty_comm_connect();
	        printf("\n CMD_SYNC_END ");
	        break;

		default:
			printf("\n UNDEFINED COMMAND ");
			break;

	}


	//write into file
	if(sFrame.command < (0x0F | 0x80))   	//system command?
		;
	else if(sFrame.command < (0x1F | 0x80))  //IO command?
		io_write_config (&aux_sIo);
	else if(sFrame.command < (0x4F | 0x80)){  //Messmodul command?
	//else if(sFrame.command == (CMD_MM_GET_PF_3 | 0x80)){ //Messmodul command?
		pm_write_config (&aux_sPm);
		//printf("\nzapisuji.. cmd: 0x%x", sFrame.command);
	}

	return 0;
}




int main (int argc, char *argv[]){
	tIO sIo;

    printf("I: tty: start.. \n");
    io_default_config (&sIo);

    tty_comm_init();
    tty_comm_connect();

	printf("\nwhile..");
    while(1){
    	tty_comm_manager();
    }


    return 0;
}


