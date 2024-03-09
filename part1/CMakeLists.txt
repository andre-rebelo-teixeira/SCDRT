cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(../pico-sdk/pico_sdk_init.cmake)

project(scdtr_part1)

## have to set this before including pico_sdk_init
## in order to have exceptions
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Split the space-separated list of source files into a CMake list
string(REPLACE " " ";" SOURCE_FILES "${SRCS}")

# Add the source files to the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include the directory containing header files
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pwm hardware_adc)

pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
