Subsonic IPT
============

An Arduino Sketch for inertial position tracking using a ADXL345 accelerometer.

This repository was developed by the "Subsonic" team as part of an assignment GE 1501 course at Northeastern University during Fall 2019.

Installation
------------

This sketch using the `Sparkfun ADXL345 Arduino Library`_ to interface with the ADXL345 accelerometer. The simplest way to install this library is via the library manager included with the Ardunio IDE. Instructions for how to install this library using the library manager can eb found in the `SparkFun tutorial`_ for installing Arduino libraries.

.. _Sparkfun ADXL345 Arduino Library: https://github.com/sparkfun/SparkFun_ADXL345_Arduino_Library
.. _SparkFun tutorial: https://learn.sparkfun.com/tutorials/installing-an-arduino-library

Running the Sketch
------------------

This repository is designed to be built and run using CMake. However, it is still possible to run this sketch using the Arduino IDE.

Running with CMake
^^^^^^^^^^^^^^^^^^

First, ensure that you have ``cmake`` installed on your system. If you are using an IDE such as `CLion`_, ``cmake`` would have been included with your installation.

Next, update the ``arduino-variables.cmake`` file to reflect your local Arduino installation. The file included in this repository represents an Arduino v1.8.10 installation in ``/opt`` on a Linux system. If you are using an operating system other than Linux, you may need to look up the appropriate system paths for your Arduino installation. For MacOS users, you may find the `arduino-clion-minimal
`_ repository which inspired the structure of this repository helpful.

If you are using `CLion`_, follow the steps given in `this blog post`_ to configure your IDE's toolchain.

.. _arduino-clion-minimal : https://github.com/cpbotha/arduino-clion-minimal
.. _this blog post: https://vxlabs.com/2018/03/24/developing-arduino-sketches-with-jetbrains-clion-a-minimal-example/
.. _CLion: https://www.jetbrains.com/clion/

Otherwise, you can build the project manually with `cmake` by running the following commands:

.. code-block:: shell

    $ mkdir cmake-build-debug
    $ cd !$
    $ cmake -DCMAKE_TOOLCHAIN_FILE=arduino-uno-toolchain.cmake ..

To build the sketch, run the `verify` CMake target.
To upload the sketch to your Arduino, run the `upload` CMake target.

Running with Arduino IDE
------------------------

To run this sketch using the Arduino IDE, rename the ``sketch.cpp`` file to have an ``.ino`` extension. Then, open this repository as you would any other Arduino sketch.



Credits
-------

The structure of repository is a custom Linux port of a template created by Charl P. Botha. His original ``arduino-clion-minimal`` repository may be found `here`_.

.. _here: https://github.com/cpbotha/arduino-clion-minimal

License
-------

This software is licensed under the `MIT License`_. For more
information, read the file `LICENSE`_.

.. _MIT License: https://opensource.org/licenses/MIT
.. _LICENSE: ./LICENSE
