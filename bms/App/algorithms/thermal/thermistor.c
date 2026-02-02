#include "thermistor.h"
#include "bms_types.h"
#include "parse.h"

// TODO: Do testing and generate more accurate values
//thermistor_t temperature_resistance_lut[] = {
//    {-50, 125000}, {-25, 90000}, {0, 11700}, {25, 10000}, {50, 3000},
//    {75, 1100},    {100, 1000},  {125, 400}, {150, 200},
//};
thermistor_t temperature_resistance_lut[] = {
  // Exponential curve fit using original values
    {-50, 109143},
    {-49, 105627},
    {-48, 102223},
    {-47, 98930},
    {-46, 95742},
    {-45, 92657},
    {-44, 89672},
    {-43, 86783},
    {-42, 83987},
    {-41, 81281},
    {-40, 78662},
    {-39, 76127},
    {-38, 73674},
    {-37, 71301},
    {-36, 69003},
    {-35, 66780},
    {-34, 64628},
    {-33, 62546},
    {-32, 60531},
    {-31, 58580},
    {-30, 56693},
    {-29, 54866},
    {-28, 53099},
    {-27, 51388},
    {-26, 49732},
    {-25, 48130},
    {-24, 46579},
    {-23, 45078},
    {-22, 43626},
    {-21, 42220},
    {-20, 40860},
    {-19, 39543},
    {-18, 38269},
    {-17, 37036},
    {-16, 35843},
    {-15, 34688},
    {-14, 33570},
    {-13, 32489},
    {-12, 31442},
    {-11, 30429},
    {-10, 29448},
    {-9, 28500},
    {-8, 27581},
    {-7, 26693},
    {-6, 25833},
    {-5, 25000},
    {-4, 24195},
    {-3, 23415},
    {-2, 22661},
    {-1, 21931},
    {0, 21224},
    {1, 20540},
    {2, 19878},
    {3, 19238},
    {4, 18618},
    {5, 18018},
    {6, 17438},
    {7, 16876},
    {8, 16332},
    {9, 15806},
    {10, 15297},
    {11, 14804},
    {12, 14327},
    {13, 13865},
    {14, 13418},
    {15, 12986},
    {16, 12568},
    {17, 12163},
    {18, 11771},
    {19, 11392},
    {20, 11025},
    {21, 10669},
    {22, 10326},
    {23, 9993},
    {24, 9671},
    {25, 9359},
    {26, 9058},
    {27, 8766},
    {28, 8483},
    {29, 8210},
    {30, 7946},
    {31, 7690},
    {32, 7442},
    {33, 7202},
    {34, 6970},
    {35, 6745},
    {36, 6528},
    {37, 6318},
    {38, 6114},
    {39, 5917},
    {40, 5727},
    {41, 5542},
    {42, 5363},
    {43, 5191},
    {44, 5023},
    {45, 4862},
    {46, 4705},
    {47, 4553},
    {48, 4407},
    {49, 4265},
    {50, 4127},
    {51, 3994},
    {52, 3866},
    {53, 3741},
    {54, 3620},
    {55, 3504},
    {56, 3391},
    {57, 3282},
    {58, 3176},
    {59, 3074},
    {60, 2975},
    {61, 2879},
    {62, 2786},
    {63, 2696},
    {64, 2609},
    {65, 2525},
    {66, 2444},
    {67, 2365},
    {68, 2289},
    {69, 2215},
    {70, 2144},
    {71, 2075},
    {72, 2008},
    {73, 1943},
    {74, 1881},
    {75, 1820},
    {76, 1761},
    {77, 1705},
    {78, 1650},
    {79, 1597},
    {80, 1545},
    {81, 1495},
    {82, 1447},
    {83, 1401},
    {84, 1355},
    {85, 1312},
    {86, 1269},
    {87, 1229},
    {88, 1189},
    {89, 1151},
    {90, 1114},
    {91, 1078},
    {92, 1043},
    {93, 1009},
    {94, 977},
    {95, 945},
    {96, 915},
    {97, 885},
    {98, 857},
    {99, 829},
    {100, 803},
    {101, 777},
    {102, 752},
    {103, 727},
    {104, 704},
    {105, 681},
    {106, 659},
    {107, 638},
    {108, 618},
    {109, 598},
    {110, 578},
    {111, 560},
    {112, 542},
    {113, 524},
    {114, 507},
    {115, 491},
    {116, 475},
    {117, 460},
    {118, 445},
    {119, 431},
    {120, 417},
    {121, 403},
    {122, 390},
    {123, 378},
    {124, 366},
    {125, 354},
    {126, 343},
    {127, 331},
    {128, 321},
    {129, 310},
    {130, 300},
    {131, 291},
    {132, 281},
    {133, 272},
    {134, 264},
    {135, 255},
    {136, 247},
    {137, 239},
    {138, 231},
    {139, 224},
    {140, 217},
    {141, 210},
    {142, 203},
    {143, 196},
    {144, 190},
    {145, 184},
    {146, 178},
    {147, 172},
    {148, 167},
    {149, 161},
    {150, 156},
};

resistance_readings_t get_resistance_from_voltage(voltage_readings_t voltage) {
  float v_ref = 3.0F; // Reference voltage for thermistors
  float v_t = convert_voltage_human_readable(voltage);
  return (resistance_readings_t)((10000 * v_t) / (v_ref - v_t));
}

temperature_readings_t get_temp_from_resistance(resistance_readings_t resistance) {
  // Binary Search algorithm
  size_t num_lut_elems = sizeof(temperature_resistance_lut) /
                         sizeof(temperature_resistance_lut[0]);

  size_t high = num_lut_elems - 1;
  size_t low = 0;
  size_t mid = (high + low) / 2;

  if (resistance >= temperature_resistance_lut[low].resistance) {
    return temperature_resistance_lut[low].temperature;
  }

  if (resistance <= temperature_resistance_lut[high].resistance) {
    return temperature_resistance_lut[high].temperature;
  }

  while (low <= high) {
    mid = (high + low) / 2;

    if (resistance == temperature_resistance_lut[mid].resistance) {
      return temperature_resistance_lut[mid].temperature;
    }

    if (temperature_resistance_lut[mid].resistance > resistance) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  return temperature_resistance_lut[low].temperature;
}

temperature_readings_t get_temp_from_voltage(voltage_readings_t voltage) {
  resistance_readings_t resistance = get_resistance_from_voltage(voltage);
  return get_temp_from_resistance(resistance);
}
