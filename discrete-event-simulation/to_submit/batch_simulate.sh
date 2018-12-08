#!/bin/sh
STOPTIME=500000
qtype=$1    # prefix, either 0, -1, -2
PARDIR=$2  # parent directory if any
CPU=$3 # number of CPUs
function simulate () {
    INPUT=$1
    PRFX="${INPUT:0:2}"
    echo prefix $PRFX
    FIN=$(echo $INPUT | sed -E 's/^.{2}//')
    if [ $PRFX -eq -3 ]; then
        PRFX=Reinforcement
        Q=-3
        ODIR=$PARDIR/DES_RL
    else
        printf -v Q "%04d" $INPUT
        ODIR=$PARDIR/DESQuantum__$Q
        PRFX=Constant
    fi
    echo $Q
    echo QUANTUM $Q
    echo $ODIR
    CMD="python simulator.py --stop-time $STOPTIME -q $Q -o $ODIR -c $CPU -N 15 -w 1 -v 100 -m"
    echo $CMD
    echo $CMD | sh
}

if [ $qtype -eq 0 ]; then
    for i in 0 1 `seq 10 5 200`; do
        simulate $i
    done
else
    simulate $qtype
fi

