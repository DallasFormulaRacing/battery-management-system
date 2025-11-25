#ifndef __ENUMS_H
#define __ENUMS_H

typedef enum {
  DISABLED = 0,
  ENABLED = 1,
} LOOP_MEASUREMENT_ENABLE;

typedef enum {
  AUX_ALL = 0,
  GPIO1,
  GPIO2,
  GPIO3,
  GPIO4,
  GPIO5,
  GPIO6,
  GPIO7,
  GPIO8,
  GPIO9,
  GPIO10,

  VREF2,
  LD03V,
  LD05V,
  TEMP,
  V_POSTIVE_2_NEGATIVE,
  V_NEGATIVE,
  VRES,
  VREF3,
} CHANNEL_SELECT;

typedef enum {

} READ_DEVICE;

typedef enum {

}

#endif