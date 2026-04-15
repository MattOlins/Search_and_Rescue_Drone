cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(pico_sdk_import.cmake)

project(drone_mpu6050 C CXX ASM)

pico_sdk_init()

add_executable(drone_mpu6050
        dronempu.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(drone_mpu6050 pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(drone_mpu6050)