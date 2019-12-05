Subsonic IPT
============

An Arduino Sketch for inertial position tracking using an MPU6060 accelerometer and gyroscope.

This repository was developed by the "Subsonic" team as part of an assignment for the GE 1501 course at Northeastern University during Fall 2019.

Installation
------------

To install the latest release of this sketch, simply clone the git repository by running

.. code-block:: shell

    $ git clone https://github.com/blueschu/Subsonic-IPT.git

This sketch depends on the Arduino LiquidCrystal library. If this library was not included with your Arduino distribution, you can install if by following the steps outlined in the `SparkFun Tutorial`_ for installing Arduino libraries.

.. _SparkFun tutorial: https://learn.sparkfun.com/tutorials/installing-an-arduino-library

Running the Sketch
------------------

This repository is designed to be built and run using CMake. However, it is still possible to run this sketch using the Arduino IDE.

Running with CMake
^^^^^^^^^^^^^^^^^^

First, ensure that you have ``cmake`` installed on your system. If you are using an IDE such as `CLion`_, ``cmake`` would have been included with your installation.

Next, update the ``arduino-variables.cmake`` file to reflect your local Arduino installation. The file included in this repository represents an Arduino v1.8.10 installation in ``/opt`` on a Linux system. If you are using an operating system other than Linux, you may need to look up the appropriate system paths for your Arduino installation. For MacOS users, you may find the `arduino-clion-minimal`_ repository which inspired the structure of this repository helpful.

If you are using `CLion`_, follow the steps given in `this blog post`_ to configure your IDE's toolchain.

.. _arduino-clion-minimal: https://github.com/cpbotha/arduino-clion-minimal
.. _this blog post: https://vxlabs.com/2018/03/24/developing-arduino-sketches-with-jetbrains-clion-a-minimal-example/
.. _CLion: https://www.jetbrains.com/clion/

Otherwise, you can build the project manually with `cmake` by running the following commands:

.. code-block:: shell

    $ mkdir cmake-build-debug
    $ cd !$
    $ cmake -DCMAKE_TOOLCHAIN_FILE=arduino-uno-toolchain.cmake ..

To build the sketch, run the ``verify`` CMake target.
To upload the sketch to your Arduino, run the ``upload`` CMake target.

Running with the Arduino IDE
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
