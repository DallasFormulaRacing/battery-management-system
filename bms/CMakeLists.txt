# root cmake
cmake_minimum_required(VERSION 3.22)
set(CMAKE_BUILD_TYPE Debug)


option(BUILD_TESTING "Build native executable for unit testing" OFF)

# ezfd CAN bootloader: link the app at 0x08008000 (STM32G474XX_FLASH_BL.ld)
# and move the vector table there. Set by the Debug-BL / Release-BL presets.
# Off (Debug / Release) = the usual image at 0x08000000, flashed over SWD.
# See bootloader/README.md.
option(BOOTLOADER "Build the app to be loaded by the ezfd CAN bootloader" OFF)
if(BOOTLOADER)
  add_compile_definitions(BMS_BOOTLOADER=1 VECT_TAB_OFFSET=0x8000U)
  set(BMS_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/STM32G474XX_FLASH_BL.ld")
else()
  add_compile_definitions(BMS_BOOTLOADER=0)
  set(BMS_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/STM32G474XX_FLASH.ld")
endif()

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


add_subdirectory(Middlewares)
add_subdirectory(App)

if(BUILD_TESTING)
  message(STATUS ">>> Building for TESTING - skipping ARM target <<<")
  include(CTest)
  enable_testing()
  add_subdirectory(submodules/external/unity_cmake)
  add_subdirectory(Tests)
  return()
endif()

message(STATUS ">>> Building for ARM target <<<")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux-specific stuff
endif () 

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # MacOS-specific stuff
  set(CMAKE_SYSROOT /Applications/ArmGNUToolchain/14.3.rel1/arm-none-eabi)
  set(CMAKE_C_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")
  set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")
endif () 



file(GLOB CORE_SOURCES Core/Src/*.c)

add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    startup_stm32g474xx.s
)

if(BOOTLOADER)
  # Image header (board tag) and end marker the bootloader checks before it
  # starts the app. Added here, not in a library, so the linker keeps them.
  target_sources(${PROJECT_NAME} PRIVATE App/config/app_image.c)
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32G474xx
    USE_HAL_DRIVER
)



target_include_directories(${PROJECT_NAME} PRIVATE
    Core/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32G4xx/Include
    Drivers/STM32G4xx_HAL_Driver/Inc
    App/services/program     
)
add_link_options(--specs=nosys.specs)


target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
    -O0 -g3            
)

file(GLOB HAL_SOURCES Drivers/STM32G4xx_HAL_Driver/Src/*.c)
target_sources(${PROJECT_NAME} PRIVATE ${HAL_SOURCES})

# 10. Linker Settings
target_link_options(${PROJECT_NAME} PRIVATE
    "-T${BMS_LINKER_SCRIPT}"
    -Wl,--gc-sections
    --specs=nosys.specs 
)



target_link_libraries(${PROJECT_NAME}
    PRIVATE
        core_layer
        cb
        adbms  
        segment
        charger
        program    
        daemons    
        threads
        gui
        ev27_bms
        ev27can
        bms_watchdog
        bms_limits
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMENT ">> Running command: OBJCOPY to BIN format"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf
    COMMENT ">> Running command: Memory Usage Report")