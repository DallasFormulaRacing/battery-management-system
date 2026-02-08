cmake_minimum_required(VERSION 3.22)

# 1. Toolchain & Project Setup
option(BUILD_TESTING "Build native executable for unit testing" OFF)

if(NOT CMAKE_TOOLCHAIN_FILE AND NOT BUILD_TESTING)
  set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_LIST_DIR}/cmake/gcc-arm-none-eabi.cmake"
        CACHE FILEPATH "Toolchain file"
  )
endif()

project(bms C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. Process App Layer
# This defines bms_api and its sub-libraries (spi, adbms, cb, etc.)
add_subdirectory(App)

# 3. Native Unit Testing Guard
if(BUILD_TESTING)
  message(STATUS ">>> Building for TESTING - skipping ARM target <<<")
  include(CTest)
  enable_testing()
  add_subdirectory(submodules/external/unity_cmake)
  add_subdirectory(Tests)
  return()
endif()

message(STATUS ">>> Building for ARM target <<<")

# 4. Workspace / Environment Settings
set(CMAKE_SYSROOT /Applications/ArmGNUToolchain/14.3.rel1/arm-none-eabi)
set(CMAKE_C_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

# 5. Executable Definition
file(GLOB CORE_SOURCES Core/Src/*.c)

add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    startup_stm32g474xx.s
)

# 6. Global Compiler Definitions (Fixes the "Select Target Device" error)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32G474xx
    USE_HAL_DRIVER
)

# 7. Include Directories
# This allows main.c to find bms.h and HAL headers
target_include_directories(${PROJECT_NAME} PRIVATE
    Core/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32G4xx/Include
    Drivers/STM32G4xx_HAL_Driver/Inc
    App/program        # FIX: Allows #include "bms.h"
    App/math/cb        # FIX: Allows #include "cb.h" (if needed in main)
)
add_link_options(--specs=nosys.specs)

# 8. Optimization & CPU Flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
    -O0 -g3            # Development defaults
)

# 9. HAL & Driver Sources
# We add them to the executable here
file(GLOB HAL_SOURCES Drivers/STM32G4xx_HAL_Driver/Src/*.c)
target_sources(${PROJECT_NAME} PRIVATE ${HAL_SOURCES})

# 10. Linker Settings
target_link_options(${PROJECT_NAME} PRIVATE
    "-T${CMAKE_SOURCE_DIR}/STM32G474XX_FLASH.ld"
    -Wl,--gc-sections
    --specs=nosys.specs # Often needed for embedded GCC
)

# 11. Final Dependency Linkage
# This connects the main executable to your entire logic tree
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        core_layer
        cb
        adbms  # <--- Linking this here tells main.c where config.h is
        segment
        charger
        bms_app
)