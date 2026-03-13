/**
 * @file iso_parse.h
 * @author Will K
 * @brief
 * @version 0.1
 * @date 2026-03-12
 *
 * @copyright no
 *
 */
#ifndef ISO_PARSE_H
#define ISO_PARSE_H

#include "can.h"
#include "iso_types.h"

void imd_parse_general();
void imd_parse_isolation();
void imd_parse_voltage();
void imd_parse_IT_system();

#endif