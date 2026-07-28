#ifndef SOC_CURVE_H
#define SOC_CURVE_H

#define SOC_LUT_SIZE 21

// example idk

static const float soc_curve_cell_voltages[SOC_LUT_SIZE] = {
    3.00F, 3.06f, 3.12f, 3.18f, 3.24f, 3.30F, 3.36f, 3.42f, 3.48f, 3.54f,
    3.60F, 3.66f, 3.72f, 3.78f, 3.84f, 3.90F, 3.96f, 4.02f, 4.08f, 4.14f, 4.20F
};

static const float soc_curve_percentages[SOC_LUT_SIZE] = {
    0.0F,   5.0F,   10.0F,  15.0F,  20.0F,  25.0F,  30.0F,  40.0F,  50.0F,  58.0F,
    65.0F,  72.0F,  78.0F,  83.0F,  88.0F,  92.0F,  95.0F,  97.0F,  99.0F,  99.5f, 100.0F
};


#endif