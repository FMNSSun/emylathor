#!/bin/bash

if [ $# -lt 3 ] ; then
	echo "Usage: " $0 "<file1> <file2> <logfile>"
	exit -1
else
	file1=$1
	file2=$2
	logfile=$3
fi

echo "----------------------------------------------------"
echo "Testing if files $1 and $2 are equal"
echo "If errors occur, see: $3"
echo "----------------------------------------------------"

diff $file1 $file2 --suppress-common-lines -w -i -B > $logfile 2>&1
ret_code=$?

if [ $ret_code -eq 0 ]
then
   echo "Success: $file1 and $file2 have the same content"
   exit 0
elif [ $ret_code -eq 1 ] ; then
   echo "ERROR: $file1 and $file2 differ"
   exit 1
else
   echo "There was something wrong with the diff command"
   exit 2
fi