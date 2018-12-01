import sys, os
import random
import argparse
from desutils import RandomNumberGenerator, DiscreteEventSimulator

def natural_num_gt0(x):
    """make `x` an int, verify `x` > 0"""
    x = int(x)
    if x > 0:
        return x
    raise ValueError

def natural_num_inc0(x):
    """make `x` an int, verify `x` > 0"""
    x = int(x)
    if x >= 0:
        return x
    raise ValueError

def exists(x):
    """make sure x is an existing file"""
    x = str(x)
    if os.path.exists(x):
        return x
    raise FileNotFoundError

def validquantum(x):
    """
    verify the parameter is a valid quantum
    """
    UNIFORM = -1
    EXPONENTIAL = -2
    x = int(x)
    if x < 0:
        qtype = int(str(x)[:2])
        value = natural_num_gt0(int(str(x)[2:]))
        if qtype == UNIFORM:
            return ('u',value)
        if qtype == EXPONENTIAL:
            return ('e',value)
        else:
            raise ValueError
    else:
        x = natural_num_inc0(x)
        return ('c',x)

def parse_args():
    D = 'Discrete Event Simulation: model processes propagating through a scheduling system'
    parser = argparse.ArgumentParser(description=D)


    parser.add_argument('--procgen-file',
                        '-f',
                        dest='procgen',
                        default='pg.txt',
                        type=exists,
                        help='File name for the process description file')

    parser.add_argument('--num-cpus','-c',
                        dest='num_cpus',
                        default=1,type=natural_num_gt0,
                        help='Number of CPUs')

    parser.add_argument('--quantum','-q',
                        dest='quantum',
                        default=0,
                        help='Quantum for pre-emptive scheduling;'+
                             'specify int > 0 for constant quantum, '+
                             '0 for no quantum, '+
                             '-1u, u an integer for quantums drawn from U(1,u), '+
                             '-2e, e an integer for quantums drawn from exponential dist. with mean e',
                        type=validquantum)

    parser.add_argument('--stop-time','-t',
                        dest='stop_time',
                        type=natural_num_gt0,
                        default=100,
                        help='Simulation stop time')
    
    parser.add_argument('--switch-time','-w',
                        dest='ctx_switch',
                        help='Context switch cost',
                        default=0,
                        type=natural_num_inc0)

    parser.add_argument('--no-io-faults','-n',
                        dest='disable_io',
                        action='store_true',
                        default=False,
                        help='Disable I/O faults')

    parser.add_argument('--verbose','-v',
                        dest='verbose',
                        help='Enable verbose output',
                        action='store_true',
                        default=False)

    parser.add_argument('--seed','-s',
                        dest='seed',
                        type=int,
                        default=None,
                        help='Seed for random number generation') 

    parser.add_argument('--mersenne-twister','-m',
                        dest='mers',
                        action='store_true',
                        default=False,
                        help='Use more current pseudorandom number generation method as implemented in `random`')
    return parser

args = parse_args().parse_args()

QUANTUM, SWITCHTIME, STOPTIME = args.quantum, args.ctx_switch, args.stop_time
        
if __name__ == '__main__':
    try:
        # the random number generator for the simulation
        rng = RandomNumberGenerator(seed=args.seed)
        # if mersenne twister was specified
        if args.mers:
            if args.seed:
                random.seed(args.seed)
            rng.override_rand(random.random) 
        args.rng = rng
        # filter out arguments we don't need, initialize the clock
        args.enable_io = not args.disable_io
    except KeyboardInterrupt:
        print('\n[X] Quitting')

    try:
        des_params = {arg:getattr(args, arg) for arg in vars(args) if arg in DiscreteEventSimulator.params}
        system_clock = DiscreteEventSimulator(**des_params)
        system_clock.initialize()
        while system_clock < system_clock.STOPTIME:
            occurred = system_clock.handle_event()
            if args.verbose:
                print(occurred)
        print()
        print(system_clock)
    except KeyboardInterrupt:
        print('\n')
        print(system_clock)
        print('\n[X] Quitting', file=sys.stderr)
