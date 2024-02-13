#!/bin/sh
set -ex
llc samples/test.ll -opaque-pointers -relocation-model=pic -code-model=large
gcc samples/test.s -o samples/test.out
echo RUNNING BINARY:
set +e
samples/test.out
echo $?