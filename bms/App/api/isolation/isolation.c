#include "isolation.h"

void imd_init(){
    configure_imd_params();
    configure_imd_cyclic();
}

void imd_update(){
    IMD_Data_t data = imd_get_data();
}

