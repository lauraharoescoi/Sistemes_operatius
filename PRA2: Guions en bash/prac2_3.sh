#!/bin/bash

if [$# -lt 2]
then
    echo "Ús: $0 <nombre1> <nombre2> [... <nombreN>]"
    exit 1
fi
 
echo "M E N U "
echo "------- "
echo "X: maXim "
echo "N: miNim "
echo "P: Primers "
echo "Tria una opció: "

read var

case $var in
    X)

    ;;
    N)

    ;;
    P)

    ;;
    *)
    echo "Opció '$var' no vàlida"
    exit 1
    ;;
esac

Maxim() {
    maxim=0
    while [$# -ne 0]
    do
        if ["$1" -ge "$maxim"]
        then
            maxim=$1
        fi
        shift
    done
    return $maxim
}

Minim() {
    minim=$1
    while [$# -ne 0]
    do
        if ["$1" -le "$minim"]
        then
            minim=$1
        fi
        shift
    done
    return $minim
}

CheckPrimary() {
    for ((i=2; i<=$1/2; i++))
    do 
        if [$(($1%i)) -ne 0]
        then
            return 1
        fi
    done
    return 0
}

Primer() {
    primers=""
    while [$# -ne 0]
    do
        if [CheckPrimary -ne 1]
        then
            primers=$primers + "$1"
        fi
        shift
    done
    return $primers    
}

