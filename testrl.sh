#!/bin/bash
echo "Running the testsuite for emulathor [rlang version]"
echo "Please be patient... this may take a while."
echo "-------------------------------------------"
fails=""
failed=0

rm -rf ./bin/tmp/
mkdir ./bin/tmp/
mkdir ./bin/tmp/tests

for filer in ./src/tests/rlang/*.rl
do
	fbaser=$(basename $filer)
	fnamer=${fbaser%.*}
	echo Compiling [$fnamer]
	./invrlang.sh $filer ./src/rlang/ ./bin/tmp/tests/$fnamer.V
        if [ $? -ne 0 ]; then
		echo "Compilation failed"
		exit 1
	fi
done

for file in ./bin/tmp/tests/*.V
do
	fbase=$(basename $file)
	fname=${fbase%.*}
	echo Running Test [$fname]

	if [ -e "./src/tests/rlang/$fname.txt" ] ; then
		cat "./src/tests/rlang/$fname.txt"
	fi

	./vasm -v $file -i bin/tests/$fbase.img -f 4096 > bin/logs/$fname.log && ./va bin/tests/$fbase.img >> bin/logs/$fname.log
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



if [ $failed -eq 0 ] ; then
	echo "ALL TESTS PASSED"
else
	echo "SOME TESTS FAILED"
	echo "Namely: $fails"
	echo "Please report them as bugs"
fi
#
