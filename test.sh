#!/bin/bash
echo "Running the testsuite for emulathor"
echo "Please be patient... this may take a while."
echo "-------------------------------------------"
fails=""
failed=0
for file in ./src/tests/*.V
do
	fbase=$(basename $file)
	fname=${fbase%.*}
	echo Running Test [$fname]
	./vasm --asm $file --img bin/tests/$fbase.img > bin/logs/$fname.log && ./va bin/tests/$fbase.img >> bin/logs/$fname.log
	retcode=$?
	
	if [ $fname = "fail" ] ; then
		if [ $retcode -ne 3 ] ; then
			echo "TEST FAILED! with exit code " $retcode
			fails=$fails,$fname
			failed=1
		fi
	else
		
		if [ $retcode -ne 0 ] ; then
			echo "TEST FAILED! with exit code " $retcode
			fails=$fails,$fname
			failed=1
		fi
	fi
done
echo "Running tests that should fail"
echo "-------------------------------------------"
for file in ./src/tests/fail/*.V
do
	fbase=$(basename $file)
	fname=${fbase%.*}
	echo Running Test [$fname]
	./vasm --asm $file --img bin/tests/$fbase.img > bin/logs/$fname.log  2>&1 && ./va bin/tests/$fbase.img >> bin/logs/$fname.log 2>&1
	retcode=$?
	
	failCode=$(echo $fname | awk -F"_" '{print $1}')   
	set -- $failCode
	any=0
	if [ $failCode = 'x' ] ; then
		any=1
	fi
	
	if [  $any -ne 1 ]  && [ $retcode -ne $failCode ] ; then
		echo "TEST SUCCEEDED but should have failed!"
		fails=$fails,$fname
		failed=1
	fi
done


if [ $failed -eq 0 ] ; then
	echo "ALL TESTS PASSED"
else
	echo "SOME TESTS FAILED"
	echo "Namely: $fails"
	echo "Please report them as bugs"
fi
#