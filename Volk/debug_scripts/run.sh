#!/bin/sh
set -ex
./Volk samples/test.vk $*
llc samples/test.ll -opaque-pointers -relocation-model=pic
gcc samples/test.s -o samples/test.out
echo RUNNING BINARY:
set +e
samples/test.out
echo $?