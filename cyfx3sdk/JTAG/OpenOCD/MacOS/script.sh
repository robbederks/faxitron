#!/bin/sh

## Script to copy the library files required by openocd into the relevant system folders.
## This needs to be run as a super-user script (sudo).

## Copy the libusb-1.0 files to /usr/local/lib. Comment this section if libusb-1.0 is already installed.
echo "Copying and setting up libusb-1.0 links"
cp libusb-1.0.a /usr/local/lib
cp libusb-1.0.la /usr/local/lib
cp libusb-1.0.0.dylib /usr/local/lib
ln -s /usr/local/lib/libusb-1.0.0.dylib /usr/local/lib/libusb-1.0.dylib

## Copy the libcyusbserial files /usr/local/lib.
echo "Copying and setting up libcyusbserial links"
cp libcyusbserial.0.1.dylib /usr/local/lib
ln -s /usr/local/lib/libcyusbserial.0.1.dylib /usr/local/lib/libcyusbserial.dylib

## Make the openocd binary executable
chmod 755 openocd

echo "Done"
