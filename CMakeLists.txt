cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

# # rest of your project
project(MyProj)

# # initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(hello hello.cpp)
pico_generate_pio_header(hello ${CMAKE_CURRENT_SOURCE_DIR}/hello.pio)

target_link_libraries(hello pico_stdlib hardware_clocks hardware_pio hardware_spi hardware_dma)
