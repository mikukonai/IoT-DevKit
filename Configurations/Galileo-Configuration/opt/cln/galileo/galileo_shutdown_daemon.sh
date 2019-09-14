#!/bin/sh
keepgoing=true
counter=0
echo "running"
while $keepgoing
do
	value=`cat /sys/class/gpio/gpio53/value`
	if [ "$value" == "0" ]; then
		echo "$counter"
		let "counter=counter+1"
		flag=`expr $counter % 2`
		if [ "$flag" == "0" ]; then
			echo 1 > /sys/class/gpio/gpio3/value
		else
			echo 0 > /sys/class/gpio/gpio3/value
		fi

		if [ "$counter" == "20" ]; then
			echo `poweroff`
		fi

	else
		counter=0

	fi
	usleep 50000
done


