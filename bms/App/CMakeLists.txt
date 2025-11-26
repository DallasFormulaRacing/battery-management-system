# ! App/CMakeLists.txt
add_library(bms_core STATIC)

# Add all submodules; each one will add sources to bms
add_subdirectory(core)
add_subdirectory(algorithms)
add_subdirectory(lib)
