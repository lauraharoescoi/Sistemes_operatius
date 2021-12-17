#!/bin/bash  

if test $# -ne 2 
then
    echo "$0 suma els dos nombres passats com a parametres"
    echo "Ús: $0 <nombre1> <nombre2>"
    exit 1
fi 

let suma=$1+$2
echo "$1 + $2 = $suma" 
exit 0 




