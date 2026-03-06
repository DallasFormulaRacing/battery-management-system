add_subdirectory(adbms)
add_subdirectory(amc)
add_subdirectory(cab)
add_subdirectory(elcon)
add_subdirectory(gui)
add_subdirectory(nvm)

add_library(lib_layer INTERFACE)

target_link_libraries(lib_layer INTERFACE 
    adbms
    amc
    cab
    elcon
    gui
    nvm
)