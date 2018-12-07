import sys, os
import random
import argparse
from desutils import RandomNumberGenerator, DiscreteEventSimulator
import pickle
import matplotlib.pyplot as plt

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
    #UNIFORM = -1
    #EXPONENTIAL = -2
    REINFORCEMENT = -3
    if int(x) == REINFORCEMENT:
        return ('r','null')
    x = int(x)
    x = natural_num_inc0(x)
    return ('c',x)

def valid_dir(x):
    if x is None:
        return x
    if not os.path.isdir(x) and not os.path.exists(x):
        os.makedirs(x)
    else:
        print(f"[!!] {x} exists") 
        raise ValueError
    return x
            

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
                             #'-1u, u an integer for quantums drawn from U(1,u), '+
                             #'-2e, e an integer for quantums drawn from exponential dist. with mean e'+
                             '-3 for RL adaptive preemption approach',
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
                        help='Enable verbose output; 0 == full verbose,'+
                             '> 0 == quieter version (print every v events),'+
                             '< 0 == nothing',
                        type=int,
                        default=-1)
    
    parser.add_argument('--output-directory','-o',
                        dest='output',
                        help="where to store output files if any; "+
                             "if None, no output; if directory exits, the "+
                             "provided name + the next available 0-padded number will be "+
                             "generated instead;",
                        type=valid_dir,
                        default=None)
    
    parser.add_argument('--num-trials','-N',
                        dest='num_trials',
                        default=1,
                        type=natural_num_gt0,
                        help='number of trials to do')

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

def summarize_arguments(args,filename='args.txt'):
    """
    report the arguments passed into the app 
    """
    formatter = "[%s] %+15s"
    
    keys = vars(args).keys()
    max_arglen = max([len(k) for k in keys])
    
    formatter = "[%{}s] %40s".format(max_arglen)
    summ_str = ""
    for k in keys:
        summ_str += formatter % (k, vars(args)[k]) + '\n'
    arg_summary = os.path.join(args.output, filename)

    with open(arg_summary, 'w') as summary:
        print(summ_str, file=summary)

def main(args,t=0):
    # if mersenne twister was specified
    if args.mers:
        if args.seed:
            random.seed(args.seed)
        rng.override_rand(random.random) 
    args.rng = rng
    if args.verbose > 0:
        def pprint(x, i=0):
            if i % args.verbose == 0:
                print('\r{0}\r{1}'.format(80*" ",occurred),end='',flush=True)
            if x is None:
                print('\r{0}\r'.format(80*" "))

    elif args.verbose == 0:
        def pprint(x,i=0):
            if x is None:
                return
            print(x)
    else:
        def pprint(x,i=0):
            pass
    try:
        des_params = {arg:getattr(args, arg) for arg in vars(args) if arg in DiscreteEventSimulator.params}
        system_clock = DiscreteEventSimulator(**des_params)
        system_clock.initialize()
        ev = 0
        while system_clock < system_clock.STOPTIME:
            occurred = system_clock.handle_event()
            pprint(occurred, i=ev)
            ev+=1
        pprint(None)
        print()
        if args.output is not None:
            with open(os.path.join(args.output, 'pickles','DiscreteEventSimulator_Trial{:03d}.bin'.format(t)),'wb') as pkl:
                pickle.dump(system_clock, pkl)


        if args.output is not None:
            print(system_clock)
            with open(os.path.join(args.output,'trials','DES_Summary_trial{:03d}.txt'.format(t)),'w') as txt:
                print(system_clock, file=txt)

    except KeyboardInterrupt:
        print('\n')
        print(system_clock)
        raise KeyboardInterrupt

#def plot(self):
#    """
#    plot throughput over time, histogram of time quantums per process
#    """
#    self.finalize()
#    # throughput
#    for ptype in self.factory.process_types:
#       pass 

if __name__ == '__main__':
    args = parse_args().parse_args()
    os.makedirs(os.path.join(args.output, 'pickles'))
    os.makedirs(os.path.join(args.output, 'trials'))
    try:
        # the random number generator for the simulation
        rng = RandomNumberGenerator(seed=args.seed)
        # filter out arguments we don't need, initialize the clock
        args.enable_io = not args.disable_io
        if args.output is not None:
            summarize_arguments(args)
    except KeyboardInterrupt:
        print('\n[X] Quitting',file=sys.stderr)
    
    try:
        for trial in range(args.num_trials):
            main(args, t=trial)
    except KeyboardInterrupt:
        print('\n[X] Quitting',file=sys.stderr)

    # now generate plots that average over each trial
    
