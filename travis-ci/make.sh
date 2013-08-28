#!/bin/sh
set -e

cd build && make -j2 && make -j2 && exit 0
