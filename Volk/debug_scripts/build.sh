#!/bin/sh
set -ex
./Volk samples/test.vk $*
llc samples/test.ll -opaque-pointers -relocation-model=pic -code-model=large
gcc samples/test.s -o samples/test.out