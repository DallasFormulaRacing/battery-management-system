add_subdirectory(charger)
add_subdirectory(current)
add_subdirectory(eeprom)
add_subdirectory(interface)
add_subdirectory(isolation)
add_subdirectory(precharge)
add_subdirectory(segment)

add_library(api_layer INTERFACE)

target_link_libraries(api_layer INTERFACE 
    charger 
    current 
    eeprom 
    interface 
    isolation
    precharge 
    segment
)