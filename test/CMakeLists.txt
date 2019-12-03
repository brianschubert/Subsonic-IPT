file(GLOB ARDUINO_SOURCES ../src/*)

add_executable(tests test.cpp ${ARDUINO_SOURCES})
