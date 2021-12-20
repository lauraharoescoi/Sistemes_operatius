# ------------------------------
# PRA2: Guions en bash
# Codi font: prac2_3.sh
# Laura Haro Escoi
# Jonàs Salat Torres
# ------------------------------

#!/bin/bash

#funcions

Maxim() {
    max=$1
    while [ $# -ne 0 ]
    do
        if [ "$1" -ge "$max" ]
        then
            max=$1
        fi
        shift
    done
    return $max
}

Minim() {
    min=$1
    while [ $# -ne 0 ] 
    do
        if [ "$1" -le "$min" ]
        then
            min=$1
        fi
        shift
    done
    return $min
}

CheckPrimary() {
    if [ $1 -le 1 ]
    then
        return 1
    fi

    for (( i=2; i<=$1/2; i++ ))
    do 
        if [ $(($1 % i)) -eq 0 ]
        then
            return 1
        fi
    done
    return 0
}

Primers() {
    while [ $# -ne 0 ]
    do
        CheckPrimary $1
        if [ $? -ne 1 ]
        then
            primers="${primers} $1"
        fi
        shift
    done
    echo "$primers"    
}

#main

if [ $# -lt 2 ]
then
    echo "Ús: $0 <nombre1> <nombre2> [... <nombreN>]"
    exit 1
fi
 
echo "M E N U "
echo "------- 
"
echo "X: maXim "
echo "N: miNim "
echo "P: Primers 
"
echo "Tria una opció: "

read var

case $var in
    X|x)
        Maxim $@
        echo "Maxim de [$@] : $?"
        exit 0
    ;;
    N|n)
        Minim $@
        echo "Minim de [$@] : $?"
        exit 0
    ;;
    P|p)
        echo "Nombres primers de [$@]: $(Primers $@)"
        exit 0
    ;;
    *)
    echo "Opció '$var' no vàlida"
    exit 1
    ;;
esac

