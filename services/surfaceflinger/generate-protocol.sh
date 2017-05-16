#!/bin/sh
cd $1

wayland-scanner client-header < wayland-android.xml > DisplayHardware/wayland-android-client-protocol.h
wayland-scanner code < wayland-android.xml > DisplayHardware/wayland-android-client-protocol.c

