#!/bin/sh

set -xe

qemu-system-x86_64 -boot d -chardev stdio,id=char0,logfile=com1.log,signal=off -serial chardev:char0 -cdrom ./build/os.iso -m 512
