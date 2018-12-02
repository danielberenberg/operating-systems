#!/bin/sh
STOPTIME=100000
PRFX=$1
function simulate () {
    Q=$1
    echo QUANTUM $Q
    python simulator.py --stop-time $STOPTIME -q $Q -o DES_Quantum$Q -c 4 -N 10 -w 1 -v 100
}

if [ $PRFX -eq 0 ]; then
    for i in 0 1 `seq 10 10 200`; do
        simulate $i
    done
else
    for i in 1 `seq 10 10 200`; do
        simulate $PRFX$i
    done

fi

