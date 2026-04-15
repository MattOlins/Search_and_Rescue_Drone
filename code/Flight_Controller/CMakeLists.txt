cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(pico_sdk_import.cmake)

project(fight_controller C CXX ASM)

pico_sdk_init()

add_executable(fight_controller
        bar180f.c
        controller.c
        dronempu.c
        dshot_encoder.cpp
        mixer.c
        pico_pio_loader.cpp
        pid.c
        test_main.cpp
        timer.cpp
        )

# pull in common dependencies
target_link_libraries(fight_controller
        hardware_pwm 
        pico_stdlib
        pico_bootsel_via_double_reset
        hardware_pwm
        hardware_pio 
        hardware_sync
        hardware_gpio
        hardware_i2c
)
pico_generate_pio_header(fight_controller ${CMAKE_CURRENT_LIST_DIR}/dshot_encoder.pio)

# create map/bin/hex file etc.
pico_add_extra_outputs(fight_controller)

