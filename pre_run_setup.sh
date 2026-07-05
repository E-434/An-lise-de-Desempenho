#!/usr/bin/env bash

set -e

clear

chmod +x run_full_factorial.sh

sudo chmod -R a+r /sys/class/powercap/intel-rapl

rm -f results/*

systemd-inhibit \
    --what=idle:sleep:shutdown \
    --why="Rodando benchmark" \
    ./run_full_factorial.sh