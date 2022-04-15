#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make server client -C $DIR/
alacritty -e $DIR/run_server &
sleep 0.3s
alacritty -e $DIR/run_client &
