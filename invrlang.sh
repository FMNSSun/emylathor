#!/bin/sh

tmpnam=`tempfile`
echo $tmpnam
cpp $1 -v -I $2 -E -x none > $tmpnam
./rlang $tmpnam > $3
if [ $? -ne 0 ] ; then
  echo "Compiler error"
  exit 1
fi
echo $tmpnam
