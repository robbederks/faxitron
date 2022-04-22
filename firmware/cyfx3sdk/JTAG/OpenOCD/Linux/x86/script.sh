#!/bin/sh

## Script to copy the library files required by openocd into the relevant system folders.
## This needs to be run as a super-user script (sudo).

## Copy the libusb-0.1 files to /usr/local/lib. Comment this section if libusb-0.1 is already installed.
echo "Copying and setting up libusb-0.1 links"
cp libusb.a /usr/local/lib
cp libusb.la /usr/local/lib
cp libusb-0.1.so.4.4.4 /usr/local/lib
ln -s /usr/local/lib/libusb-0.1.so.4.4.4 /usr/local/lib/libusb-0.1.so.4
ln -s /usr/local/lib/libusb-0.1.so.4.4.4 /usr/local/lib/libusb-0.1.so

## Copy the libusb-1.0 files to /usr/local/lib. Comment this section if libusb-1.0 is already installed.
echo "Copying and setting up libusb-1.0 links"
cp libusb-1.0.a /usr/local/lib
cp libusb-1.0.la /usr/local/lib
cp libusb-1.0.so.0.1.0 /usr/local/lib
ln -s /usr/local/lib/libusb-1.0.so.0.1.0 /usr/local/lib/libusb-1.0.so.0
ln -s /usr/local/lib/libusb-1.0.so.0.1.0 /usr/local/lib/libusb-1.0.so

## Copy the libcyusbserial files /usr/local/lib.
echo "Copying and setting up libcyusbserial links"
cp libcyusbserial.so.1 /usr/local/lib
ln -s /usr/local/lib/libcyusbserial.so.1 /usr/local/lib/libcyusbserial.so

## Create the udev rules that make the device nodes writable.
cp 91-cyusbserial.rules /etc/udev/rules.d

## Make the openocd binary executable
chmod 755 openocd

echo "Done"
