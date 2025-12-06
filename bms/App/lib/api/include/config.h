#ifndef CONFIG_H
#define CONFIG_H

#include "bms_types.h"

#define IC_COUNT_CHAIN 1
#define WRITE_SIZE (ADBMS_TX_FRAME_BYTES * IC_COUNT_CHAIN)

#define CELL 16          /* Bms ic number of cell              */
#define AUX 12           /* Bms ic number of Aux               */
#define RAUX 10          /* Bms ic number of RAux              */
#define PWMA 12          /* Bms ic number of PWMA              */
#define PWMB 4           /* Bms ic number of PWMB              */
#define COMM 3           /* GPIO communication comm reg        */
#define RSID 6           /* Bms ic number of SID byte          */
#define TX_DATA 6        /* Bms tx data byte                   */
#define RX_DATA 8        /* Bms rx data byte                   */
#define RDCVALL_SIZE 34  /* RDCVALL data byte size             */
#define RDSALL_SIZE 34   /* RDSALL data byte size              */
#define RDACALL_SIZE 34  /* RDACALL data byte size             */
#define RDFCALL_SIZE 34  /* RDFCALL data byte size             */
#define RDCSALL_SIZE 66  /* RDCSALL data byte size             */
#define RDASALL_SIZE 70  /* RDASALL data byte size             */
#define RDACSALL_SIZE 66 /* RDACSALL data byte size            */

cell_asic_ctx_t asic_ctx[IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];

#endif