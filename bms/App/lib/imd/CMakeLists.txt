# Bender iso175C IMD on the tb bus (FDCAN1): Info_General and the threshold check.
add_library(imd STATIC)

target_sources(imd PRIVATE src/imd.c)

target_include_directories(imd PUBLIC inc)
target_link_libraries(imd PUBLIC ev27can freertos)
