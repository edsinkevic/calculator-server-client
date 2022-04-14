#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make multiserver client -C $DIR/
alacritty -e $DIR/run_multiserver &
alacritty -e $DIR/run_client &
alacritty -e $DIR/run_client &
