#!/bin/sh
set -e

mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_LIBIPSTACK_WITHOUT_KCONFIG=1 -BUILD_LIBIPSTACK_WITH_ALL_FILES=1 ../ && exit 0