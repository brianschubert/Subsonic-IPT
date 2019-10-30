# Copyright (c) 2019 Brian Schubert.

# Env Variables specifying the properties of your Arduino installation.
# Customizing these for your system should be straightforward.
set(ARDUINO_INSTALL_ROOT /opt/arduino-1.8.10)
set(ARDUINO_USER_LIBRARIES $ENV{HOME}/Arduino/libraries)
set(ARDUINO_INSTALL_LIBRARIES ${ARDUINO_INSTALL_ROOT}/libraries)
set(ARDUINO_CMD ${ARDUINO_INSTALL_ROOT}/arduino)
