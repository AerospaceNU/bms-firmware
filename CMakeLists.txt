# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bms-firmware C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(bms-firmware logic/src/fw_logic_main.c)

pico_set_program_name(bms-firmware "bms-firmware")
pico_set_program_version(bms-firmware "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bms-firmware 0)
pico_enable_stdio_usb(bms-firmware 1)

# Add the standard library to the build
target_link_libraries(bms-firmware
        pico_stdlib)

# Add any user requested libraries
target_link_libraries(bms-firmware 
        hardware_spi)

# Add the standard include files to the build
target_include_directories(bms-firmware PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/drivers/inc
  ${CMAKE_CURRENT_LIST_DIR}/logic/inc
  ${CMAKE_CURRENT_LIST_DIR}/utils/inc
)

# Collect all source files from drivers, logic, and utils directories
file(GLOB DRIVERS_SRC "${CMAKE_CURRENT_LIST_DIR}/drivers/src/*.c")
file(GLOB LOGIC_SRC   "${CMAKE_CURRENT_LIST_DIR}/logic/src/*.c")
file(GLOB UTILS_SRC   "${CMAKE_CURRENT_LIST_DIR}/utils/src/*.c")

# Add these files to your executable target
target_sources(bms-firmware PRIVATE ${DRIVERS_SRC} ${LOGIC_SRC} ${UTILS_SRC})

pico_add_extra_outputs(bms-firmware)