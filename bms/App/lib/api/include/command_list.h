#ifndef COMMAND_LIST_H
#define COMMAND_LIST_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t command_t[2];
typedef uint8_t long_command_t[13];

/* configuration registers commands */
static const command_t WRCFGA = {0x00, 0x01};
static const command_t WRCFGB = {0x00, 0x24};
static const command_t RDCFGA = {0x00, 0x02};
static const command_t RDCFGB = {0x00, 0x26};

/* Read cell voltage result registers commands */
static const command_t RDCVA = {0x00, 0x04};
static const command_t RDCVB = {0x00, 0x06};
static const command_t RDCVC = {0x00, 0x08};
static const command_t RDCVD = {0x00, 0x0A};
static const command_t RDCVE = {0x00, 0x09};
static const command_t RDCVF = {0x00, 0x0B};
static const command_t RDCVALL = {0x00, 0x0C};

/* Read average cell voltage result registers commands */
static const command_t RDACA = {0x00, 0x44};
static const command_t RDACB = {0x00, 0x46};
static const command_t RDACC = {0x00, 0x48};
static const command_t RDACD = {0x00, 0x4A};
static const command_t RDACE = {0x00, 0x49};
static const command_t RDACF = {0x00, 0x4B};
static const command_t RDACALL = {0x00, 0x4C};

/* Read s voltage result registers commands */
static const command_t RDSVA = {0x00, 0x03};
static const command_t RDSVB = {0x00, 0x05};
static const command_t RDSVC = {0x00, 0x07};
static const command_t RDSVD = {0x00, 0x0D};
static const command_t RDSVE = {0x00, 0x0E};
static const command_t RDSVF = {0x00, 0x0F};
static const command_t RDSALL = {0x00, 0x10};

/* Read c and s results */
static const command_t RDCSALL = {0x00, 0x11};
static const command_t RDACSALL = {0x00, 0x51};

/* Read all AUX and all Status Registers */
static const command_t RDASALL = {0x00, 0x35};

/* Read filtered cell voltage result registers */
static const command_t RDFCA = {0x00, 0x12};
static const command_t RDFCB = {0x00, 0x13};
static const command_t RDFCC = {0x00, 0x14};
static const command_t RDFCD = {0x00, 0x15};
static const command_t RDFCE = {0x00, 0x16};
static const command_t RDFCF = {0x00, 0x17};
static const command_t RDFCALL = {0x00, 0x18};

/* Read aux results */
static const command_t RDAUXA = {0x00, 0x19};
static const command_t RDAUXB = {0x00, 0x1A};
static const command_t RDAUXC = {0x00, 0x1B};
static const command_t RDAUXD = {0x00, 0x1F};

/* Read redundant aux results */
static const command_t RDRAXA = {0x00, 0x1C};
static const command_t RDRAXB = {0x00, 0x1D};
static const command_t RDRAXC = {0x00, 0x1E};
static const command_t RDRAXD = {0x00, 0x25};

/* Read status registers */
static const command_t RDSTATA = {0x00, 0x30};
static const command_t RDSTATB = {0x00, 0x31};
static const command_t RDSTATC = {0x00, 0x32};
static const command_t RDSTATCERR = {0x00, 0x72}; /* ERR */
static const command_t RDSTATD = {0x00, 0x33};
static const command_t RDSTATE = {0x00, 0x34};

/* PWM registers commands */
static const command_t WRPWM1 = {0x00, 0x20};
static const command_t RDPWM1 = {0x00, 0x22};

static const command_t WRPWM2 = {0x00, 0x21};
static const command_t RDPWM2 = {0x00, 0x23};

/* Clear commands */
static const command_t CLRCELL = {0x07, 0x11};
static const command_t CLRAUX = {0x07, 0x12};
static const command_t CLRSPIN = {0x07, 0x16};
static const command_t CLRFLAG = {0x07, 0x17};
static const command_t CLRFC = {0x07, 0x14};
static const command_t CLOVUV = {0x07, 0x15};

/* Poll ADC commands */
static const command_t PLADC = {0x07, 0x18};
static const command_t PLAUT = {0x07, 0x19};
static const command_t PLCADC = {0x07, 0x1C};
static const command_t PLSADC = {0x07, 0x1D};
static const command_t PLAUX1 = {0x07, 0x1E};
static const command_t PLAUX2 = {0x07, 0x1F};

/* Diagn command */
static const command_t DIAGN = {0x07, 0x15};

/* GPIOs Comm commands */
static const command_t WRCOMM = {0x07, 0x21};
static const command_t RDCOMM = {0x07, 0x22};
static const long_command_t STCOMM_FULL = {0x07, 0x23, 0xB9, 0xE4, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00};

static const command_t STCOMM = {0x07, 0x23};

/* Mute and Unmute commands */
static const command_t MUTE = {0x00, 0x28};
static const command_t UNMUTE = {0x00, 0x29};

static const command_t RSTCC = {0x00, 0x2E};
static const command_t SNAP = {0x00, 0x2D};
static const command_t UNSNAP = {0x00, 0x2F};
static const command_t SRST = {0x00, 0x27};

/* Read SID command */
static const command_t RDSID = {0x00, 0x2C};

#endif
