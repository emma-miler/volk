#!/bin/sh
set -ex
./Volk samples/test.vk --ir_verbosity 0
llc samples/test.ll -opaque-pointers -relocation-model=pic
gcc samples/test.s -o samples/test.out
echo RUNNING BINARY:
set +e
samples/test.out
echo $?