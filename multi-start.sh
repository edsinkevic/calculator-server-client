#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make -C $DIR/
alacritty -e $DIR/run_multiserver &
alacritty -e $DIR/run_client &
alacritty -e $DIR/run_client &
