# Adapted Charl P. Botha's arduino-clion-minimal template.
#
# Copyright (c) 2019 Brian Schubert.
# Copyright (c) 2018 Charl P. Botha <cpbotha@vxlabs.com>.

cmake_minimum_required(VERSION 3.9)

project(Subsonic-IPT)

set(CMAKE_CXX_STANDARD 17)

include(arduino-variables.cmake)

# Add any additional library include dirs your project needs here
# Note: the ${ARDUINO_INSTALL_ROOT}/hardware* includes are NOT REQUIRED
# to build your project. However, they are needed here for CLion to provide
# adequate code completion for the arduino headers.
include_directories(
        # Redundant libraries for Clion code completion
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/cores/arduino
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/variants/standard
        ${ARDUINO_INSTALL_ROOT}/hardware/tools/avr/avr/include/
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/libraries/Wire/src
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/cores/
        ${ARDUINO_INSTALL_ROOT}/hardware/tools/avr/avr/include/avr
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/libraries/SPI/src

        # Project-specified added libraries
        ${ARDUINO_USER_LIBRARIES}/SparkFun_SerLCD_Arduino_Library/src
)

file(GLOB ARDUINO_SOURCES src/*)
# use this target for a quick compile-only check of your CPP files
# add more source (CPP) files here when you add them to the sketch
add_executable(arduino-clion-minimal sketch.cpp ${ARDUINO_SOURCES})
set_target_properties(arduino-clion-minimal PROPERTIES LINKER_LANGUAGE CXX)

# use the following two targets for building and uploading sketches from clion.
# select "verify" for just building, select "upload" for building and uploading.
# make sure you have selected the correct board and port in the arduino IDE before building this target!
# instead, you can also use --board and --port arguments here. for the m0, --board arduino:samd:mzero_bl
# see https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc for arduino cmd options
add_custom_target(upload ALL ${ARDUINO_CMD} --upload --preserve-temp-files --verbose blink.ino WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_custom_target(verify ALL ${ARDUINO_CMD} --verify --preserve-temp-files --verbose blink.ino WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add test target
add_subdirectory(test)
