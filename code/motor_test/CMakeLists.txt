cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")


include(pico_sdk_import.cmake)

project(sensor_test_project C CXX ASM)

pico_sdk_init()

add_executable(sensor_test_project
        dshot_motor_test.cpp
        timer.cpp
        dshot_encoder.cpp
        pico_pio_loader.cpp
        )

# pull in common dependencies
target_link_libraries(sensor_test_project 
        hardware_pwm 
        pico_stdlib
        pico_bootsel_via_double_reset
        hardware_pwm
         hardware_pio 
        hardware_sync


)
pico_generate_pio_header(sensor_test_project ${CMAKE_CURRENT_LIST_DIR}/dshot_encoder.pio)

# create map/bin/hex file etc.
pico_add_extra_outputs(sensor_test_project)

