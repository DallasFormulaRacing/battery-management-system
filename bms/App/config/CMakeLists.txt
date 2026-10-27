# App/config/CMakeLists.txt
# header-only, no dependencies so any layer can include it
add_library(bms_limits INTERFACE)
target_include_directories(bms_limits INTERFACE .)
