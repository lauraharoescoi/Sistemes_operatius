# ------------------------------
# PRA2: Guions en bash
# Codi font: prac2_4.sh
# Laura Haro Escoi
# Jonàs Salat Torres
# ------------------------------

#!/bin/bash

users=$(cut -d ":" -f1,7 /etc/passwd)
if [ $# -eq 0 ]
then
	for line in $users
	do 
		if [[ $line == *"nologin"* ]]
		then 
			echo "	${line%:*} NO connectat actualment"
		else
			process=$(ps -u ${line%:*} | wc -l)
			echo "	${line%:*} connectat actualment amb $process processos en execucio"
		fi
	done		
else
	while test $# -ne 0 
	do
		user=`grep "^$1:" /etc/passwd`
		
		if [[ $user == *"nologin"* ]]
		then 
			echo "	$1 NO connectat actualment"
		else
    			process=$(ps -u $1 | wc -l)
    			echo "	$1 connectat actualment amb $process processos en execucio"
    		fi	
    		shift
	done
fi
exit 0