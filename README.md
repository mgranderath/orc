# OpenWrt RESTCONF

This is a prototype implementation of RESTCONF for the OpenWrt system that utilizes the UCI configuration files as a
datastore.

## Requirements

1. The OpenWrt Buildroot has to be installed on the system ([Tutorial](https://openwrt.org/docs/guide-developer/build-system/use-buildsystem))
2. Two environment variables have to be set. An example of them can be seen in `.env.example`.
3. Python 3 for the YANG conversion script
4. Tool for converting YANG to YIN

## Adding YANG modules

To add YANG modules for OpenWrt they have to go through some pre-processing. This is
what the `main.py` script does.

1. Convert the YANG modules to be included to YIN and put them in one folder, i.e. `/yin`
2. Run the `main.py` script
   ```console
   python3 main.py -y ./yin -o ./generated restconf-example.yin ...
   ```
   This converts the YIN files and generates a `.h` file that has to be included in `/src/generated/yang.h`

## Building

1. Create a new directory `mkdir build`
2. Change into the directory `cd build`
3. Run cmake `cmake ..`
4. Run make `make`

After this a `restconf` binary will have compiled that can then be used as a CGI
script on OpenWrt

## Architecture

![Architecture](docs/resources/Architecture.png)