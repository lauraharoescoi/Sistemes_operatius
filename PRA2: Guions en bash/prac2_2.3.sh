# ------------------------------
# PRA2: Guions en bash
# Codi font: prac2_2.3.sh
# Laura Haro Escoi
# Jonàs Salat Torres
# ------------------------------

#!/bin/bash  

if test $# -ne 2 
then
    echo "$0 suma els dos nombres passats com a parametres"
    echo "Ús: $0 <nombre1> <nombre2>"
    exit 1
fi 

echo "$1 + $2 = $(($1+$2))" 
exit 0 

