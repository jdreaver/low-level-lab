#!/usr/bin/env bash

set -eux

rm -rf build

for config in Release Debug; do
    cmake -S . -B "build/$config" "-DCMAKE_BUILD_TYPE=$config"
done
