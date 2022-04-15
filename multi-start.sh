#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make multiserver client -C $DIR/
alacritty -e $DIR/run_multiserver &
sleep 0.3s
alacritty -e $DIR/run_client &
alacritty -e $DIR/run_client &
