#!/bin/bash
Cmd=$1
Log=$2
while [ 1 ]; do
	$Cmd 2>&1  |	while read line; do
		echo $(date) $line >> $Log
	done
	sleep 1
done
