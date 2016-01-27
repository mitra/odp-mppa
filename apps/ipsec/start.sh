#!/bin/sh
[ -z "$K1_TOOLCHAIN_DIR" ] && K1_TOOLCHAIN_DIR="/usr/local/k1tools"
MYDIR="$(readlink -f "$(dirname "$0")")"
sudo sh -c 'echo 600 > /mppa/board0/mppa0/chip_freq'
while sleep 1; do
	stdbuf -oL "$MYDIR/run.sh" "$MYDIR/ipsec.py"
done
