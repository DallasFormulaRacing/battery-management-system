# ! App/CMakeLists.txt
# add_library(bms_api STATIC)

# Add all submodules; each one will add sources to bms
add_subdirectory(core)
add_subdirectory(lib)
add_subdirectory(math)
add_subdirectory(api)
add_subdirectory(program)
