add_library(bms_app STATIC)

# Add all submodules; each one will add sources to bms_core
add_subdirectory(core)
add_subdirectory(algorithms)
add_subdirectory(lib)
