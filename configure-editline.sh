#!/bin/bash -ex
for i in $*; do
  echo $i
  eval "export $i"
done
rsync $SRC/editline/ . --delete -r
./autogen.sh
./configure --prefix=$DST/editline