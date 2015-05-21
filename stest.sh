#!/bin/sh
echo "Baking cookies..."
make va
make vasm
echo "Running a single test for emulathor $1"
echo "-------------------------------------------"
fn="./src/tests/$1"
echo $fn
fbase=$(basename $fn)
fname=${fbase%.*}
echo Running Test [$fname]
./vasm --asm $fn --img bin/tests/$fbase.img > bin/logs/$fname.log && ./va bin/tests/$fbase.img >> bin/logs/$fname.log
retcode=$?
if [ $retcode -ne 0 ] ; then
	echo "TEST FAILED! with exitcode " $retcode
else
	echo "TEST PASSED! with exitcode " $retcode
fi
