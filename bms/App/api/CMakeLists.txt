add_subdirectory(charger)
add_subdirectory(current)
add_subdirectory(eeprom)
add_subdirectory(interface)
add_subdirectory(packvolt)
add_subdirectory(segment)

add_library(api_layer INTERFACE)

target_link_libraries(api_layer INTERFACE 
    charger 
    current 
    eeprom 
    interface 
    packvolt 
    segment
)