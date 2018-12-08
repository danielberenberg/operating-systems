"""
script for generating plots
"""

from glob import glob
import sys, os
import matplotlib.pyplot as plt
import pickle
import numpy as np
import desutils

ptypes = ['batch', 'interactive']

def simfile_locations(filepaths):
    """
    return the locations of simulation files
    """
    par2simfiles = dict()
    for parent_dir in filepaths:
        par2simfiles[parent_dir] = sorted(list(filter(os.path.isdir, glob(os.path.join(parent_dir, '*')))))

    return par2simfiles

def get_pickle(filename):
    """
    return a serialized version of a python object
    """
    with open(filename, 'rb') as pk:
        return pickle.load(pk)

def get_sims(simfile_dir):
    """
    return the set of simulations found in a simulation directory
    """
    sims = sorted(glob(os.path.join(simfile_dir, "pickles/*")))
    print(f"\r{80*' '}\r{simfile_dir}", end='',flush=True)
    return map(get_pickle, sims)

def cache(handler):
    """
    decorator to do arbitrary pickle caching
    args:
        :handler (function) - returns the data if it is not cached
        args:
            :pkf (str) - path to check for caching
            :*args - the arguments that handler takes
        returns:
            :the object that will be pickled at pkf
    """
    def wrapper(pkf, *args):
        if not isinstance(pkf, str):
            raise ValueError
        try:
            with open(pkf, 'rb') as pk:
                print(f"{pkf} is cached!")

        except FileNotFoundError:
            o = handler(*args)
            with open(pkf, 'wb') as pk:
                pickle.dump(o, pk)
                print(f"cached content to {pkf}")
            return o
    return wrapper

@cache
def max_throughput(exp_class):
    """
    find the throughput per time trend for the 
    top quantum in the experiment class 
    (i.e, constant Q=25, 3 cpus) 
    """
    top_procs_completed = 0
    top_class_idx = None
    top_class = None
    for i, cls in enumerate(exp_class):
        avg_procs_completed = 0
        num_sims = 0
        for sim in get_sims(cls):
            avg_procs_completed += sim.processes_completed
            num_sims += 1

        avg_procs_completed /= num_sims
        if avg_procs_completed > top_procs_completed:
            top_class = cls
            top_class_idx = i
            top_procs_completed = avg_procs_completed

    return _throughput(get_sims(exp_class[top_class_idx])), top_class

def _throughput(simulation_collection):
    ptype2throughputs = {ptype:dict() for ptype in ptypes}

    ptype_tp = dict()
    for sim in simulation_collection:
        for ptype in ptypes:
            for t in sim.process_stats[ptype]['throughput']:
                try:
                    ptype_tp[t].append(sim.process_stats[ptype]['throughput'][t])
                except KeyError:
                    ptype_tp[t] = [sim.process_stats[ptype]['throughput'][t]]

    for ptype in ptype2throughputs:
        for t in ptype_tp:
            ptype2throughputs[ptype][t] = (np.mean(ptype_tp[t]), np.std(ptype_tp[t]))
    return ptype2throughputs

@cache
def param_per_t(q, param):
    """
    given a parameter (say, throughput), 
    returns the set of throughputs for a given class of experiments (q)
    """
    
    L = [_param_per_t(get_sims(qdir), param) for qdir in q]
    return L

def _param_per_t(sims, param):
    aggregator = dict()
    for sim in sims:
        for t in sim.time_parameterized_process_stats:
            t_proc_stats = sim.time_parameterized_process_stats[t]
            for ptype in ptypes:
                try:
                    if isinstance(t_proc_stats[ptype][param], int):
                        aggregator[t][ptype].append(t_proc_stats[ptype][param])
                    else:
                        aggregator[t][ptype].extend(t_proc_stats[ptype][param])
                except KeyError:
                    aggregator[t] = {ptype:[] for ptype in ptypes}
                    if isinstance(t_proc_stats[ptype][param], int):
                        aggregator[t][ptype].append(t_proc_stats[ptype][param])
                    else:
                        aggregator[t][ptype].extend(t_proc_stats[ptype][param])
    for t in aggregator:
        for ptype in ptypes:
            if aggregator[t][ptype]:
                aggregator[t][ptype] = np.mean(aggregator[t][ptype])
            else:
                aggregator[t][ptype] = None
    return aggregator, ptypes

def per_process_param_per_t(ppt, ptypes, avg=False):
    """
    args:
        :ppt (param per t) - dictionary mapping time to the param in question
        :ptypes (process types)
        :avg - find the average across processes or do it on a per proc basis
    """
    if avg:
        time = []
        param = []
        for t in ppt:
            if any(ppt[t][ptype] for ptype in ptypes):
                time.append(t)
                param.append(np.mean([ppt[t][ptype] for ptype in ptypes if ppt[t][ptype]]))
        return time, param

    ptype2time_param = {ptype:[[],[]] for ptype in ptypes}
    for t in ppt:
        for ptype in ptypes:
            if ppt[t][ptype]:
                ptype2time_param[ptype][0].append(t)
                ptype2time_param[ptype][1].append(ppt[t][ptype])

    for ptype in ptypes:
        ptype2time_param[ptype][0] = sorted(ptype2time_param[ptype][0])
        ptype2time_param[ptype][1] = [ppt[t][ptype] for t in ptype2time_param[ptype][0]]
    return ptype2time_param

def avg_final_n(param, n=1000):
    return np.mean(param[-n:])

if __name__ == '__main__':
    FILEPATHS = ["batchInteractive_1cpu",
                 "batchInteractive_3cpu",
                 "batchInteractive_1cpu_rl",
                 "batchInteractive_3cpu_rl"]
    
    simfile_locs = simfile_locations(FILEPATHS)
    print('wait time')
    par2wt = {par:param_per_t(os.path.join(par, 'wait_time.bin'),
                              simfile_locs[par], 'wait_time') for par in simfile_locs}

    print(par2wt)
    print('preemptions')
    par2preemp = {par:param_per_t(os.path.join(par, 'preemptions.bin'),
                                  simfile_locs[par], 'preemptions') for par in simfile_locs}

    print(par2preemp)
    print('turnaround')
    par2turnaround = {par:param_per_t(os.path.join(par, 'turnaround.bin'),
                                      simfile_locs[par], 'turnaround') for par in simfile_locs}
    print(par2turnaround)
    print('throughputs')
    max_tp = {par:max_throughput(os.path.join(par, 'max_throughput.bin'),simfile_locs[par]) for par in simfile_locs}

    print(max_tp)
