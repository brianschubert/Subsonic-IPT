# Adapted from Charl P. Botha's arduino-clion-minimal template.
#
# Copyright (c) 2019 Brian Schubert.
# Copyright (c) 2018 Charl P. Botha <cpbotha@vxlabs.com>.

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

include(arduino-variables.cmake)

# on MacOS, this should be: $ENV{HOME}/Library/Arduino15/packages
SET(ARDUINO_PACKAGES $ENV{HOME}/.arduino15/packages)


# Specify the cross compiler
set(CMAKE_C_COMPILER ${ARDUINO_INSTALL_ROOT}/hardware/tools/avr/bin/avr-gcc)
set(CMAKE_CXX_COMPILER ${ARDUINO_INSTALL_ROOT}/hardware/tools/avr/bin/avr-g++)

# Use arduino --verify --verbose command, then wait for FINAL cpp compilation to extract these arguments, the include
# directories below this, and the definitions below that.
# hint: look in the output for "Compiling sketch..."
SET(COMMON_FLAGS "-c -g -Os -w -ffunction-sections -fdata-sections -MMD -flto -mmcu=atmega328p")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++1z -fpermissive -fno-exceptions -fno-threadsafe-statics -fno-rtti -fno-exceptions")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11 -fno-fat-lto-objects")

include_directories(
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/cores/arduino
        ${ARDUINO_INSTALL_ROOT}/hardware/arduino/avr/variants/standard
)

add_definitions(
        -DF_CPU=16000000L
        -DARDUINO=10805
        -DARDUINO_AVR_UNO
        -DARDUINO_ARCH_AVR
)
