#!/bin/bash

DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
make -C $DIR/multi-server/
sleep 0.3s
alacritty -e $DIR/multi-server/main &
sleep 0.3s
make -C $DIR/client/
sleep 0.3s
alacritty -e $DIR/client/main &
sleep 0.3s
make -C $DIR/client/
sleep 0.3s
alacritty -e $DIR/client/main &
