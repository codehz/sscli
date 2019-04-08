#!/bin/bash
for i in "${@}"; do
  eval export ${i%=*}=\"${i#*=}\"
done
set -ex
rsync $SRC/editline/ . --delete -r
./autogen.sh
./configure --prefix=$DST/editline