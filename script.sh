#!/bin/bash
# rm result.csv

# for no_threads in {1..3}
# do
# 	result=""
# 	for j in {1..5}
# 	do
# 		time=`(time ./mfind -p $no_threads /etc config) 2>&1 > /dev/null | grep real | cut -c 8-12`
# 		result+=$time";"
# 	done
# 	echo "$result" >> result.csv
# done


for no_threads in {1..100}
do
	result=""
	for j in {1..5}
	do
		time=`(time ./mfind -p $no_threads /pkg comsol) 2>&1 > /dev/null | grep real | cut -c 8-12`
		result+=$time";"
	done
	echo "$result" >> result.csv
done
