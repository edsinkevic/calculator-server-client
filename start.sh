#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make -C $DIR/
alacritty -e $DIR/run_server &
alacritty -e $DIR/run_client &
