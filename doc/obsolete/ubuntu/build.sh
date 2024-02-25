#!/bin/sh

DIR=./StationTV

mkdir -p $DIR/usr/local/bin/
cp ../stationTV $DIR/usr/local/bin/
dpkg-deb --build $DIR

