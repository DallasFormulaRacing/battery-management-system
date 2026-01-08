cmake_minimum_required(VERSION 3.22)

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

# Platform-independent sources (Always build)
add_subdirectory(App)
add_subdirectory(DumbExample)

# For native unit testing
if(BUILD_TESTING)
  message(STATUS ">>> Building for TESTING - skipping ARM target <<<")
  include(CTest)
  enable_testing()
  add_subdirectory(submodules/external/unity_cmake)
  add_subdirectory(Tests)
  return()
endif()

message(STATUS ">>> Building for ARM target <<<")

# ! Change this as necessary for your system

# Sysroot for macOS users
set(CMAKE_SYSROOT /Applications/ArmGNUToolchain/14.3.rel1/arm-none-eabi)
# Sysroot for linux users
# set(CMAKE_SYSROOT /usr/arm-none-eabi/)

set(CMAKE_C_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

# CubeMX-generated application sources
file(GLOB CORE_SOURCES
    Core/Src/*.c
)

file(GLOB HAL_SOURCES
    Drivers/STM32G4xx_HAL_Driver/Src/*.c
)

add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    ${HAL_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        Core/Inc
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32G4xx/Include
    SYSTEM PRIVATE
        Drivers/STM32G4xx_HAL_Driver/Inc
)

# Compiler defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32G474xx
    USE_HAL_DRIVER
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
    --sysroot=${CMAKE_SYSROOT}
)

if (NOT STATIC_ANALYSIS)

    target_sources(${PROJECT_NAME} PRIVATE
        startup_stm32g474xx.s
    )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            bms_api
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        "-T${CMAKE_SOURCE_DIR}/STM32G474XX_FLASH.ld"
        -Wl,--gc-sections
    )

endif()
