#!/bin/bash  

if test $# -ne 2 # treiem els parentesis, afegim test al principi de la condició i canviem el 2 per un 3
then
    echo "$0 suma els dos nombres passats com a parametres"
    echo "Ús: $0 <nombre1> <nombre2>"
    exit 1
fi # treiem el end

echo "$1 + $2 = `expr $1 + $2`" # canviem la resta per una suma
exit 0 # afegim un exit 0 per a finalitzar l'execució
