cmake_minimum_required(VERSION 3.13)

if(NOT PICO_SDK_PATH AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")
    set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk" CACHE PATH "Path to the Raspberry Pi Pico SDK" FORCE)
endif()

if(NOT PIMORONI_PICO_PATH AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/pimoroni-pico")
    set(PIMORONI_PICO_PATH "${CMAKE_CURRENT_LIST_DIR}/pimoroni-pico" CACHE PATH "Path to Pimoroni Pico libraries" FORCE)
endif()

include("${PICO_SDK_PATH}/pico_sdk_init.cmake")
include("${PIMORONI_PICO_PATH}/pimoroni_pico_import.cmake")

if(NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico_w)
endif()

project(picopro C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

include("${PIMORONI_PICO_PATH}/common/pimoroni_bus.cmake")
include("${PIMORONI_PICO_PATH}/drivers/button/button.cmake")
include("${PIMORONI_PICO_PATH}/drivers/rgbled/rgbled.cmake")
include("${PIMORONI_PICO_PATH}/drivers/uc8151/uc8151.cmake")
include("${PIMORONI_PICO_PATH}/libraries/pico_graphics/pico_graphics.cmake")
add_subdirectory(src/epaper)

add_executable(picopro
    src/main.cpp
)

target_include_directories(picopro PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src"
)

pico_enable_stdio_usb(picopro 1)
pico_enable_stdio_uart(picopro 0)
pico_add_extra_outputs(picopro)

target_link_libraries(
    picopro
    ImageData
    pico_stdlib
    hardware_spi
    hardware_pwm
    hardware_dma
    rgbled
    button
    uc8151
    pico_graphics
)
