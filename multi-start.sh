#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make multiserver client -C $DIR/
alacritty -e $DIR/bin/multiserver &
sleep 0.3s
alacritty -e $DIR/bin/client &
alacritty -e $DIR/bin/client &
