#ifndef DATA_H
#define DATA_H

#include "bms_enums.h"
#include "bms_types.h"
#include "comms.h"
#include <stdint.h>

/**
void adBmsWriteData(uint8_t tIC, cell_asic *ic, uint8_t cmd_arg[2], TYPE type,
                    GRP group) {
  uint8_t data_len = TX_DATA, write_size = (TX_DATA * tIC);
  uint8_t *write_buffer = (uint8_t *)calloc(write_size, sizeof(uint8_t));
  if (write_buffer == NULL) {
#ifdef MBED
    pc.printf(" Failed to allocate write_buffer array memory \n");
#else
    printf(" Failed to allocate write_buffer array memory \n");
#endif
    exit(0);
  } else {
    switch (type) {
    case Config:
      switch (group) {
      case A:
        adBms6830CreateConfiga(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++) {
          for (uint8_t data = 0; data < data_len; data++) {
            write_buffer[(cic * data_len) + data] =
                ic[cic].configa.tx_data[data];
          }
        }
        break;
      case B:
        adBms6830CreateConfigb(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++) {
          for (uint8_t data = 0; data < data_len; data++) {
            write_buffer[(cic * data_len) + data] =
                ic[cic].configb.tx_data[data];
          }
        }
        break;

      default:
        break;
      }
      break;

    case Comm:
      adBms6830CreateComm(tIC, &ic[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        for (uint8_t data = 0; data < data_len; data++) {
          write_buffer[(cic * data_len) + data] = ic[cic].com.tx_data[data];
        }
      }
      break;

    case Pwm:
      switch (group) {
      case A:
        adBms6830CreatePwma(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++) {
          for (uint8_t data = 0; data < data_len; data++) {
            write_buffer[(cic * data_len) + data] = ic[cic].pwma.tx_data[data];
          }
        }
        break;
      case B:
        adBms6830CreatePwmb(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++) {
          for (uint8_t data = 0; data < data_len; data++) {
            write_buffer[(cic * data_len) + data] = ic[cic].pwmb.tx_data[data];
          }
        }
        break;

      default:
        break;
      }
      break;

    case Clrflag:
      adBms6830CreateClrflagData(tIC, &ic[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        for (uint8_t data = 0; data < data_len; data++) {
          write_buffer[(cic * data_len) + data] = ic[cic].clrflag.tx_data[data];
        }
      }
      break;

    default:
      break;
    }
  }
  adBmsWakeupIc(tIC);
  spiWriteData(tIC, cmd_arg, &write_buffer[0]);
  free(write_buffer);
}
*/

comm_status_t bms_read_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                            cfg_reg_group_select_t group);

comm_status_t bms_write_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                             command_t cmd_arg, uint8_t group);

/*
void adBmsReadData(uint8_t tIC, cell_asic *ic, uint8_t cmd_arg[2], TYPE type,
                   GRP group) {
  uint16_t rBuff_size;
  uint8_t regData_size;
  if (group == ALL_GRP) {
    if (type == Rdcvall) {
      rBuff_size = RDCVALL_SIZE;
      regData_size = RDCVALL_SIZE;
    } else if (type == Rdsall) {
      rBuff_size = RDSALL_SIZE;
      regData_size = RDSALL_SIZE;
    } else if (type == Rdacall) {
      rBuff_size = RDACALL_SIZE;
      regData_size = RDACALL_SIZE;
    } else if (type == Rdfcall) {
      rBuff_size = RDFCALL_SIZE;
      regData_size = RDFCALL_SIZE;
    } else if (type == Rdcsall) {
      rBuff_size = RDCSALL_SIZE;
      regData_size = RDCSALL_SIZE;
    } else if (type == Rdasall) {
      rBuff_size = RDASALL_SIZE;
      regData_size = RDASALL_SIZE;
    } else if (type == Rdacsall) {
      rBuff_size = RDACSALL_SIZE;
      regData_size = RDACSALL_SIZE;
    } else {
      printf("Read All cmd wrong type select \n");
    }
  } else {
    rBuff_size = (tIC * RX_DATA);
    regData_size = RX_DATA;
  }
  uint8_t *read_buffer, *pec_error, *cmd_count;
  read_buffer = (uint8_t *)calloc(rBuff_size, sizeof(uint8_t));
  pec_error = (uint8_t *)calloc(tIC, sizeof(uint8_t));
  cmd_count = (uint8_t *)calloc(tIC, sizeof(uint8_t));
  if ((pec_error == NULL) || (cmd_count == NULL) || (read_buffer == NULL)) {
#ifdef MBED
    pc.printf(" Failed to allocate memory \n");
#else
    printf(" Failed to allocate memory \n");
#endif
    exit(0);
  } else {
    spiReadData(tIC, &cmd_arg[0], &read_buffer[0], &pec_error[0], &cmd_count[0],
                regData_size);
    switch (type) {
    case Config:
      adBms6830ParseConfig(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.cfgr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Cell:
      adBms6830ParseCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.cell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case AvgCell:
      adBms6830ParseAverageCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.acell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case S_volt:
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case F_volt:
      adBms6830ParseFCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.fcell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Aux:
      adBms6830ParseAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.aux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case RAux:
      adBms6830ParseRAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.raux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Status:
      adBms6830ParseStatus(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.stat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Comm:
      adBms6830ParseComm(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.comm_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Pwm:
      adBms6830ParsePwm(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.pwm_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Sid:
      adBms6830ParseSID(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.sid_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdcvall:
      // 32 byte cell data + 2 byte pec
      adBms6830ParseCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.cell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdacall:
      // 32 byte avg cell data + 2 byte pec
      adBms6830ParseAverageCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.acell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdsall:
      // 32 byte scell volt data + 2 byte pec
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdfcall:
      // 32 byte fcell data + 2 byte pec
      adBms6830ParseFCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.fcell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdcsall:
      // 64 byte + 2 byte pec = 32 byte cell data + 32 byte scell volt data
      adBms6830ParseCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.cell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[32]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdacsall:
      // 64 byte + 2 byte pec = 32 byte avg cell data + 32 byte scell volt data
      adBms6830ParseAverageCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.acell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[32]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    case Rdasall:
    // 68 byte + 2 byte pec:
    // 24 byte gpio data + 20 byte Redundant gpio data +
    // 24 byte status A(6 byte), B(6 byte), C(4 byte), D(6 byte) & E(2 byte)
      adBms6830ParseAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.aux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseRAux(tIC, ic, group, &read_buffer[24]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.raux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseStatus(tIC, ic, group, &read_buffer[44]);
      for (uint8_t cic = 0; cic < tIC; cic++) {
        ic[cic].cccrc.stat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;

    default:
      break;
    }
  }
  free(read_buffer);
  free(pec_error);
  free(cmd_count);
}*/

#endif
