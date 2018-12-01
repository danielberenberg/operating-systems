import sys
from math import log, floor
import heapq
import time

class RandomNumberGenerator(object):
    def __init__(self, seed=None):
        """
        - Implements the Rand48 functions necessary for this system
        - adapted from 
           [https://stackoverflow.com/questions/7287014/is-there-any-drand48-equivalent-in-python-or-a-wrapper-to-it]
        - Note that rand48 family of functions were declared obsolete in 1989 by SVI3
        """
        if seed:
            self.n = seed
        else:
            self.n = int(time.time())
        self.override = False

    def seed(self, seed):
        self.n = seed

    def srand(self, seed):
        self.n = (seed << 16) + 0x330e

    def override_rand(self, rng):
        """
        override the rand48 method of generating uniformly distributed numbers
        args:
            :rng (Callable)
        """
        self.override = True
        self.rng = rng

    def _next(self):
        self.n  = (25214903917 * self.n + 11) & (2**48 - 1)
        return self.n
    
    def drand(self):
        if not self.override:
            return self._next()/ 2**48
        return self.rng()

    def exprand(self, beta):
        """
        draw from an exponential distribution centered at beta
        adapted from
        [Jason Hibbeler's code, updated 11/21]
        args:
            :beta (float) - mean of exponential dist. to draw from
        """
        r = self.drand()
        t = -log(1-r) * (beta - 0.5)
        rtnval = 1 + int(floor(t))
        return rtnval

    def urand(self, a, b):
        """
        return a number drawn uniformly from the interval [a,b]
        rescaling of value n in [0, 1] to m in [a,b] can be done with the formula:
            m = (n - 0)/(1 - 0) * (b - a) + a
            m = n * (b-a) + a
             
        args:
            :a, b - floats; a < b
        returns:
            :value drawn from U(a,b)
        """
        assert a < b
        c = self.drand() * (b - a) + a
        return int(round(c))

class Process(object):
    __slots__ = ['type','burst_cpu','burst_io',
                 'demand','cpu_current','arrival_time',
                 'wait_time','num_preemptions','pid',
                 'cpu','QUANTUM']

    def __init__(self, **proc_attrs):
        """
        Initialize a Process with the specified attributes.

        Attributes should be in the above specified 
        __slots__ list or an AttributeError will be thrown. 
        
        attributes:
            :type (str) - process type
            :QUANTUM (int) - time quantum for this process
            :cpu (int) - the index of the cpu that the process is located on
            :demand (int) - total time left
            :burst_cpu (int) - burst cpu time
            :burst_io (int)  - length of io burst; if None then behavior will change,
                - namely burst_cpu implicitly becomes demand
            :cpu_current (int) - time until next io burst (if exists)
            :arrival_time (int) - time of arrival into system
            :wait_time (int) - length of time spent in ready queue
            :num_preemptions (int) - amount of time kicked off of CPU
            :pid (int) - process id
        """
        for attr in proc_attrs:
            setattr(self, attr, proc_attrs[attr])
        if not hasattr(self, 'burst_io'):
            self.cpu_current = self.burst_cpu = self.demand
        else:
            self.cpu_current = self.burst_cpu

        # wait_time and num_preemptions should be initialized to 0 if
        # they haven't already been;
        for attr in ['wait_time','num_preemptions']:
            if not hasattr(self, attr):
                setattr(self, attr, 0)
        self.cpu = None

    def __eq__(self, proc):
        if isinstance(proc, Process):
            return all(getattr(self, attr) == getattr(proc, attr) for attr in self.__slots__)
        return False

    def __str__(self):
        s = f'Process(pid={self.pid:d}|arrival={self.arrival_time}|cpu_current={self.cpu_current}|demand={self.demand}' 
        if hasattr(self,'burst_io'):
            s += f'|io_burst={self.burst_io}'
        s += ')'
        return s

class Event(object):
    PROCESS_SUBMITTED  = 'PROCESS_SUBMITTED'
    PROCESS_DISPATCHED = 'PROCESS_DISPATCHED'
    PROCESS_TERMINATED = 'PROCESS_TERMINATED'
    TIME_SLICE_EXPIRED = 'TIME_SLICE_EXPIRED'
    IO_REQUEST         = 'IO_REQUEST'
    IO_COMPLETE        = 'IO_COMPLETE'

    def __init__(self, etype, t, proc):
        """
        Initialize an event at a specific timestep that
        involves a specific process and a specific type 
        of event.

        args:
            :etype (str) - the event occurring
            :t (int) - the timestep the event takes place at
            :proc (Process) - the process that this event involves
        """
        self.type = etype
        self.t    = t
        self.p    = proc
        
    def __str__(self):
        summary = "t={0:5d} {1:>20s} {3:>5s}{2:04d}".format(self.t,self.type,self.p.pid,'pid=')
        if self.type == Event.PROCESS_TERMINATED:
            summary += " waitTime={}".format(self.p.wait_time)
        return summary #+ ' [{}]'.format(str(self.p))

    def __lt__(self, t):
        return self.t < t

    def __le__(self, t):
        return self.t <= t

    def __gt__(self, t):
        return self.t > t
    
    def __ge__(self, t):
        return self.t >= t

class CPU(object):
    BUSY = 1
    IDLE = 0

    def __init__(self, cpu_id):
        """
        Instantiate a CPU object, essentially a slot
        for processes to run in the system;
        
        Each CPU can be thought of as a core that can
        execute tasks.

        attributes:
            :id - the id of the CPU
            :active_time (int) - time spent executing jobs
            :context_switch_time (int) - counts time spent switching between processes
            :idle_time (int) - length of time spent idle
            :state - 0 == idle; 1 == busy
            :_slot - occupied/unoccupied place for proc
        """
        self.id = cpu_id
        self.active_time = self.idle_time = self.context_switch_time = 0
        self._slot = None

    @property
    def state(self):
        """
        0 --> idle
        1 --> busy
        """
        return int(self._slot is not None)

    @property
    def is_busy(self):
        return self.state == CPU.BUSY

    @property
    def is_idle(self):
        return self.state == CPU.IDLE
    
    def __call__(self, proc, ctx=0):
        self._slot = proc
        self.context_switch_time += ctx

    def __invert__(self):
        self._slot = None

    def __iter__(self):
        return self

    def __next__(self):
        params = [self.active_time, self.idle_time, self.context_switch_time]
        while params:
            yield params.pop(0)
        raise StopIteration

    def __str__(self):
        return f"CPU(active_time={self.active_time}, idle_time={self.idle_time}, occupied={self.is_busy})"

    def __repr__(self):
        return str(self)

class ProcessFactory(object):
    def __init__(self,procgen='pg.txt',rng=RandomNumberGenerator(),enable_io=True):
        """
        Creates a ProcessFactory, an object used
        to spin up new instances of processes.

        The ProcessFactory is responsible for reading
        the process generation file and validating the lines.
        Namely, each row should be space delimited 5 tuples 
        of string then 4 ints.

        Columns should be:
        name avg_demand avg_cpuburst avg_iarriv avg_io
        
        args:
            :procgen (str) - the input validated process generation file
            :rng (RandomNumberGenerator) 
        """
        self.procmap = dict()
        self.enable_io = enable_io
        self.rng = rng
        self._i = 0
        self._process_id = 0
        with open(procgen, 'r') as pgf:
            lines = map(lambda line: line.strip(), pgf.readlines())
            self.ntypes = int(next(lines))
            for i,row in enumerate(lines, 1):
                row = row.split()
                if row:
                    try:
                        name = row[0]    
                        cpu_tot, cpu_burst, iarriv, io_burst = map(int, row[1:])
                    except ValueError as e:
                        sys.exit(f'[!] [{procgen}] - Malformed process description at row {i}')
                self.procmap[name] = {'type':name, 'demand':cpu_tot,
                                      'burst_cpu':cpu_burst, 'arrival_time':iarriv}
                if self.enable_io:
                    self.procmap[name]['burst_io'] = io_burst

        self.process_types = list(self.procmap.keys())
        if self.ntypes != len(self.process_types):
            print(f'[?] Inconsistent description of file; found ntypes={self.ntypes} but number of types found is {len(self.process_types)}')

    def _observe(self, process_type, last_time, QUANTUM=1):
        """
        Spins a new process.
        CPU service time, I/O burst time, interarrival time are drawn from exponentials
        while CPU burst time is drawn from a uniform distribution in the interval 
                    [1, 2*self.procmap[process_type]['cpu_burst']].
        args:
            :process_type (str) - the process type to instantiate
            :last_time (int) - the last time a process of this type occurred
            :io (bool) - enable/disable io faults
        returns:
            :Process with demand, bursts, arrival, etc drawn from the appropriate
             distributions
        raises:
            :ValueError if process_type not in self.procmap.keys()
        """
        if process_type not in self.procmap:
            raise ValueError(f'[!] {process_type} not in {list(self.procmap.keys())}')
        
        proc_instance_params = {'type':process_type, # the name of the process
                                # cpu demand for this proc
                                'demand':self.rng.exprand(self.procmap[process_type]['demand']), 
                                # when this proc is submitted
                                'arrival_time':last_time + self.rng.exprand(self.procmap[process_type]['arrival_time']), 
                                # length of time this proc will have burst cpu
                                'burst_cpu':self.rng.urand(1, 2 * self.procmap[process_type]['burst_cpu']),
                                # pid
                                'pid':self.new_pid,
                                'QUANTUM':QUANTUM} 

        if self.enable_io:
            # length of the io burst for this proc
            proc_instance_params['burst_io'] = self.rng.exprand(self.procmap[process_type]['burst_io'])
        return Process(**proc_instance_params)

    @property
    def new_pid(self):
        self._process_id += 1
        return self._process_id - 1

    def __contains__(self, proctype):
        return proctype in self.process_types

    def __iter__(self):
        return self
    
    def __next__(self):
        try:
            self._i += 1
            return self.process_types[self._i - 1]
        except IndexError:
            self._i = 0
            raise StopIteration
            
    def __call__(self, process_type, last_time, quantum=1):
        return self._observe(process_type, last_time, QUANTUM=quantum)
        
    def __str__(self):
        return 'Process Factory ({} proc. types)'.format(len(self.procmap))


class DiscreteEventSimulator(object):
    params = ['ctx_switch',
              'enable_io',
              'quantum',
              'num_cpus',
              'stop_time',
              'rng',
              'procgen']

    def __init__(self, **kwargs):
        """
        Initializes a DiscreteEventSimulator, the universal ticker that allows the system to run.
        -----
        DiscreteEventSimulator objects contain the event and ready queues as well as the other
        parameters associated with allowing the system to run

        DES also contain the CPU slots that allow processes to run

        The DES handles events that it internally creates and processes.
        args:
            :stop_time - the maximum timestep
            :process_factory - what we will use to generate processes
            :ctx_switch - penalty for context switching
            :enable_io - should I/O faults exist?
            :quantum - lenght of time before preemption happens
            :num_cpus - number of slots to let procs run
        """
        self.CONTEXT_SWITCH = kwargs['ctx_switch']
        self.STOPTIME = kwargs['stop_time']
        self.EVENT_QUEUE = [] # Priority queue
        self.READY_QUEUE = [] # FIFO queue

        self._initialized = False
        self.factory = ProcessFactory(procgen=kwargs['procgen'],
                                      rng=kwargs['rng'],
                                      enable_io=kwargs['enable_io']) 

        qtype = kwargs['quantum'][0]
        quantum_val = kwargs['quantum'][1]
        if qtype == 'c':
            self.QUANTUM = lambda: quantum_val 
        if qtype == 'u':
            self.QUANTUM = lambda: self.factory.rng.urand(1, 2*quantum_val) 
        if qtype == 'e':
            self.QUANTUM = lambda: self.factory.rng.exprand(quantum_val)

        if self.QUANTUM() == 0:
            self.QUANTUM = lambda: None

        #self.factory = process_factory instantiate process factory
        self.enable_io = kwargs['enable_io']
        self.cpu_count = kwargs['num_cpus']
        self.T = 0
        self.T_last = self.T
        
        # statistics section of attributes
        self.ready_queue_len = []
        self.event_queue_len = []
        self.events_processed = 0
        self.processes_completed = 0
        self.timesteps = []
        # the postmortem stats for each of the processes available from the factory
        self.process_stats = {ptype:{'completed':0,
                                     'throughput':dict(), # moving average of number of processes completed per t
                                     'turnaround_times':[],
                                     'wait_time':[],
                                     'num_preemptions':[]} for ptype in self.factory.process_types}

        # initialize the slots where processes can run 
        self.CPUs = [CPU(i) for i in range(self.cpu_count)] 

    @property
    def initialized(self):
        return self._initialized

    def handle_event(self):
        """
        handle an event;
        there are six (6) different types of events, each with a different
        way they are handled
        
        1. PROCESS_SUBMITTED:  when a process enters the system 
        2. PROCESS_DISPATCHED: when a process moves running state onto an unoccupied CPU 
        3. PROCESS_TERMINATED: when a process leaves the system because its runtime has dropped to 0
        4. TIME_SLICE_EXPIRED: when the time that a process has been running exceeds the quantum
        6. IO_COMPLETE:  when I/O request finishes, moves back to ready state
        =========================================================================== 
        args:
            :event - Event 
        returns:
            :an event, if any, to add to the Event queue
        """
        # update the current time
        event = self.dequeue_event()
        self.record_status()
        self.T_last = self.T
        self.T = event.t

        # update the wait times of processes in the ready queue if there are any
        for proc in self.READY_QUEUE:
            proc.wait_time += self.T - self.T_last
        # update the cpu stats
        for cpu in self.CPUs:
            if cpu.is_busy:
                cpu.active_time += self.T - self.T_last
            else:
                cpu.idle_time += self.T - self.T_last

        if event.type == Event.PROCESS_SUBMITTED:
            self.handle_process_submitted(event)
        elif event.type == Event.PROCESS_DISPATCHED:
            self.handle_process_dispatched(event)
        elif event.type == Event.PROCESS_TERMINATED:
            self.handle_process_terminated(event)
        elif event.type == Event.TIME_SLICE_EXPIRED:
            self.handle_timeslice_expired(event)
        elif event.type == Event.IO_REQUEST:
            self.handle_io_request(event)
        elif event.type == Event.IO_COMPLETE:
            self.handle_io_complete(event)
        return event

    def handle_process_submitted(self, event):
        """
        handle PROCESS_SUBMITTED
        ------------------------
        PROCESS_SUBMITTED: 
           --> when a process enters the system 
           ------------------------------------------------------------------------
           a) If the ready queue is empty
             - assign the process to a CPU, PROCESS_DISPATCHED at the current_time
           ------------------------------------------------------------------------
           b) else enqueue the process in the ready queue
           ------------------------------------------------------------------------
           c) in both cases create a new process & PROCESS_SUBMITTED event in the 
              eventQueue @ current_time + r where r = random interarrival time
        args:
            :Event with type PROCESS_SUBMITTED
        """
        process = event.p   
        if self.idle_cpu is not None: #and self.idle_cpu is not None:
            # once this process is dispatched, i.e this event is processed, we can expect
            # to incur the penalty from context switching.
            # perform context_switch
            if not self.READY_QUEUE:
                self.T += self.CONTEXT_SWITCH
                process.cpu = self.CPUs[self.locate_cpu(self.idle_cpu)]
                self.CPUs[self.locate_cpu(process.cpu.id)](process, ctx=self.CONTEXT_SWITCH) # assign proc to a CPU
                dispatched = Event(etype=Event.PROCESS_DISPATCHED, t=self.T, proc=process)
            else:
                dispatched = Event(etype=Event.PROCESS_DISPATCHED, t=self.T, proc=self.dequeue_process())
                self.enqueue_process(process)
            self.enqueue_event(dispatched)

        else: # tripped because there is no idle cpu (self.idle_cpu = None)
            # enqueue the process into the ready queue, no event is recorded
            self.enqueue_process(process)
        # generate a new PROCESS_SUBMITTED event in the event queue
        next_proc = self.factory(process.type, self.T, quantum=self.QUANTUM()) 
        e = Event(etype=Event.PROCESS_SUBMITTED, t=next_proc.arrival_time,proc=next_proc)
        self.enqueue_event(e)

    def handle_process_dispatched(self, event):
        """
        handle PROCESS_DISPATCHED
        -------------------------
        PROCESS_DISPATCHED: 
           --> when a process moves running state onto an unoccupied CPU 
           - put a new event in the Event queue
        -------------------------------------------------------------------
        # | condition             new event             time 
        -------------------------------------------------------------------
        a | cpu_burst >  quantum | TIME_SLICE_EXPIRED | T + quantum   
        b | cpu_burst <= quantum | IO_REQUEST         | T + cpu_burst 
        c | demand <= quantum    | PROCESS_TERMINATED | T + time_remaining
        
        Context switching penalty is incurred here

        args:
            :Event with type PROCESS_DISPATCHED
        """
        process = event.p
        # want to either keep the current time cpu burst time or 
        # not overshoot the demand
        process.cpu_current = min(process.cpu_current, process.demand)
        # builtin short circuiting should trip if QUANTUM == None before evaluating second half
        if process.QUANTUM is not None and process.cpu_current > process.QUANTUM:
            # TIME_SLICE_EXPIRED @ self.T + self.QUANTUM
            e = Event(etype=Event.TIME_SLICE_EXPIRED, t=self.T + process.QUANTUM, proc=process)
            self.enqueue_event(e)
        elif process.QUANTUM is None or process.cpu_current <= process.QUANTUM:
            # in this case we can either enqueue an I/O request or terminate
            # terminate the process 
            if process.cpu_current == process.demand or process.demand == 0: 
                e = Event(etype=Event.PROCESS_TERMINATED, t=self.T + process.cpu_current, proc=process)
                self.enqueue_event(e)
            # in this case we either want to enqueue an I/O request if permissible OR
            elif self.enable_io:
                e = Event(etype=Event.IO_REQUEST, t=self.T + process.cpu_current, proc=process)
                self.enqueue_event(e)
        #else:
        # enqueue the process into the ready queue, no event is recorded
        # this happens when there was an attempt to dispatch to a nonempty CPU
        #print('[+] dispatched to an occupied CPU: {}'.format(self.CPUs))
        #    self.enqueue_process(process)

    def handle_process_terminated(self, event):
        """
        handle PROCESS_TERMINATED
        -------------------------
        PROCESS_TERMINATED:
            --> when the time that a process has been running exceeds the quantum
            - remove from CPUs
            - dequeue the next process and dispatch it
        """
        process = event.p
        process.demand = 0        
        process.cpu_current = 0
        ~self.CPUs[self.locate_cpu(process.cpu.id)]
        process.cpu = None
        # record the process post mortem stats for this process type
        self.process_stats[process.type]['completed'] += 1
        self.process_stats[process.type]['throughput'][self.T] = self.process_stats[process.type]['completed']/self.T
        self.process_stats[process.type]['turnaround_times'].append(self.T - process.arrival_time) 
        self.process_stats[process.type]['wait_time'].append(process.wait_time)
        self.process_stats[process.type]['num_preemptions'].append(process.num_preemptions)
        self.processes_completed += 1
        #del process
        if self.READY_QUEUE:
            self.T += self.CONTEXT_SWITCH
            e = Event(etype=Event.PROCESS_DISPATCHED,t=self.T, proc=self.dequeue_process())
            e.p.cpu = self.CPUs[self.locate_cpu(self.idle_cpu)]
            self.CPUs[self.locate_cpu(e.p.cpu.id)](process, ctx=self.CONTEXT_SWITCH) # assign proc to a CPU
            self.enqueue_event(e)
        else:
            print('cant dispatch',self.READY_QUEUE, self.idle_cpu)
   
    def handle_timeslice_expired(self, event):
        """
        handle TIME_SLICE_EXPIRED
        --------------------------
        TIME_SLICE_EXPIRED:
           --> when the time that a process has been running exceeds the quantum
           - increment num_preemptions for this process
           - decrease the current cpu cycle and demand by the quantum
           - enqueue the process in the ready queue
           - dequeue the next process from the ready queue, assign it to the CPU (follow process dispatched protocol)
        """
        process = event.p
        # pop process off of CPUs, to be replaced with new proc
        ~self.CPUs[self.locate_cpu(process.cpu.id)]
        process.cpu = None
        process.num_preemptions += 1
        process.demand -= process.QUANTUM 
        process.cpu_current -= process.QUANTUM
        self.T += self.CONTEXT_SWITCH
        if self.READY_QUEUE:
            e = Event(etype=Event.PROCESS_DISPATCHED,t=self.T, proc=self.dequeue_process())
            self.enqueue_process(process)
        else:
            e = Event(etype=Event.PROCESS_DISPATCHED,t=self.T,proc=process)
            
        e.p.cpu = self.CPUs[self.locate_cpu(self.idle_cpu)]
        self.CPUs[self.locate_cpu(e.p.cpu.id)](process, ctx=self.CONTEXT_SWITCH) # assign proc to a CPU
        self.enqueue_event(e)

    def handle_io_request(self, event):
        """
        handle IO_REQUEST
        -----------------
        5. IO_REQUEST:
           --> when a process in running states gets I/O request
           - update the time remaining for this process, subtracting the cpu burst time
           - put new IO_COMPLETE event in the event queue at current_time + io service time
        """
        process = event.p
        process.demand -= process.cpu_current
        ~self.CPUs[self.locate_cpu(process.cpu.id)] # open up a resource
        process.cpu = None
        # reset time remaining to next io burst
        io_complete = Event(etype=Event.IO_COMPLETE, t=self.T + process.burst_io, proc=process)
        self.enqueue_event(io_complete)
        
        # if there is a process to take up the an open resource then schedule a dispatch
        if self.READY_QUEUE:
            self.T += self.CONTEXT_SWITCH
            dispatch = Event(etype=Event.PROCESS_DISPATCHED,t=self.T, proc=self.dequeue_process())
            dispatch.p.cpu = self.CPUs[self.locate_cpu(self.idle_cpu)]
            self.CPUs[self.locate_cpu(dispatch.p.cpu.id)](dispatch, ctx=self.CONTEXT_SWITCH) # assign proc to a CPU
            self.enqueue_event(dispatch)

    def handle_io_complete(self, event):
        """
        handle IO_COMPLETE  
        ------------------
        IO_COMPLETE
            --> when I/O request finishes, moves back to ready state
            - either send this process to the ready queue or dispatch it
        """
        process = event.p
        process.cpu_current = process.burst_cpu
        # if there is an open resource
        if self.idle_cpu is not None:
            # if there is no ready procs, enqueue the one that just finished io
            self.T += self.CONTEXT_SWITCH
            if not self.READY_QUEUE:
                e = Event(Event.PROCESS_DISPATCHED, t=self.T,proc=process)
            else: # enqueue the dispatch of the next proc in the ready queue otherwise
                e = Event(Event.PROCESS_DISPATCHED, t=self.T,proc=self.dequeue_process())
                self.enqueue_process(process)
            # enequeue the event 
            e.p.cpu = self.CPUs[self.locate_cpu(self.idle_cpu)]
            self.CPUs[self.locate_cpu(e.p.cpu.id)](e, ctx=self.CONTEXT_SWITCH) # assign proc to a CPU
            self.enqueue_event(e)
        else: # otherwise mark the proc. ready
            self.enqueue_process(process)

    def initialize(self):
        """
        initialize the system by:
            1) creating the seed processes 
            2) populating the EVENT_QUEUE with the appropriate 
                PROCESS_SUBMITTED events
    
        - in order to initialize all processes, we need submit seed procs 
          at the beginning of the simulation
    
        args:
            :proc_factory - (ProcessFactory); spins up new processes 
        returns:
            :EVENT_QUEUE - initialized event queue
        """
        # submit seed processes to the event queue
        if not self.initialized:
            for i, ptype in enumerate(self.factory):
                proc_instance = self.factory(ptype, 0, quantum=self.QUANTUM())
                proc_instance.arrival_time = i  # set the submission time for seed procs
                # submit this seed proc to 
                e = Event(etype=Event.PROCESS_SUBMITTED, t=i, proc=proc_instance)
                self.enqueue_event(e)

            self._initialized = True
        else:
            print('[!!] System already initialized ...')
    
    @property
    def idle_cpu(self):
        """
        get an idle CPU; return None if none exist
        """
        self.CPUs = sorted(self.CPUs, key=lambda cpu:cpu.idle_time, reverse=True)
        if any(cpu.is_idle for cpu in self.CPUs):
            return [cpu.id for cpu in self.CPUs if cpu.is_idle][0]
        return None

    def locate_cpu(self, idx):
        for i, cpu in enumerate(self.CPUs):
            if cpu.id == idx:
                return i
    
    def enqueue_event(self, e):
        """
        push an event onto the priority queue
        args:
            :e (Event) - event with priority e.t 
        """
        heapq.heappush(self.EVENT_QUEUE,(e.t, e))

    def dequeue_event(self):
        """
        pop the top of the EventQueue, maintaining
        the heap invariant
        """
        return heapq.heappop(self.EVENT_QUEUE)[1]

    def enqueue_process(self, p):
        self.READY_QUEUE.append(p)
    
    def dequeue_process(self):
        return self.READY_QUEUE.pop(0)

    def record_status(self):
        """
        record statistics for the system
        """
        self.event_queue_len.append(len(self))
        self.ready_queue_len.append(len(self.READY_QUEUE))
        self.timesteps.append(self.T)
        self.events_processed += 1

    def finalize(self):
        """
        compute system/process based post mortem run statistics
        """
        self.SIMULATION_LEN = self.T
        self.avg_eq_len = sum(self.event_queue_len)/len(self.event_queue_len)
        self.avg_rq_len = sum(self.ready_queue_len)/len(self.ready_queue_len)
        self.final_eq_len = self.event_queue_len[-1]
        self.final_rq_len = self.ready_queue_len[-1]
        for ptype in self.factory.process_types:
            if self.process_stats[ptype]['turnaround_times']:
                turnarounds = self.process_stats[ptype]['turnaround_times']
                self.process_stats[ptype]['final_turnaround'] = turnarounds[-1]
                self.process_stats[ptype]['longest_turnaround'] = max(turnarounds)
                self.process_stats[ptype]['average_turnaround'] = sum(turnarounds)/len(turnarounds)

    def summarize(self):
        self.finalize()
        delim = '\n' + 100 * '-' + '\n'
        nl = '\n'
        summary = ""
        summary+= '=====- SIMULATION SUMMARY -=====' + nl
        summary+= f'Simulation Length: {self.SIMULATION_LEN}' + nl
        summary+= f'Total processes completed: {self.processes_completed}' + nl
        summary+= f'Total events processed: {self.events_processed}' + delim
        summary+= 'Ready Queue Statistics' + nl
        summary+= f' > average length: {round(self.avg_rq_len)}' + nl
        summary+= f' > final length  : {round(self.final_rq_len)}' + delim
        summary+= 'Event Queue Statistics' + nl
        summary+= f' > average length: {round(self.avg_eq_len)}' + nl
        summary+= f' > final length  : {round(self.final_eq_len)}' + delim
        summary+= 'Process Statistics' + nl
        summary+= '{0:<11s} - {1:<10s} - {2:<15s} - {3:<15s} - {4:<15s} - {5:<15s}'.format('Type',
                                                                                           '# Completed',
                                                                                           'Avg. turnaround',
                                                                                           'Max. turnaround',
                                                                                           'Last turnaround',
                                                                                           'Avg. wait time') + delim
        proc_strs = []
        for ptype in self.factory.process_types:
            fmt = f"{ptype:<11s} | {self.process_stats[ptype]['completed']:>11d} | " 
            mx_turnaround    = self.process_stats[ptype]['longest_turnaround']
            avg_turnaround   = self.process_stats[ptype]['average_turnaround']
            final_turnaround = self.process_stats[ptype]['final_turnaround'] 
            mean_wait_time   = sum(self.process_stats[ptype]['wait_time'])/len(self.process_stats[ptype]['wait_time'])

            try:
                fmt = fmt + "{:>15s} | ".format(f"{round(avg_turnaround)} ({quick_ratio(avg_turnaround, self.T)}%)")
                fmt = fmt + "{:>15s} | ".format(f"{final_turnaround} ({quick_ratio(final_turnaround, self.T)}%)")
                fmt = fmt + "{:>15s} | ".format(f"{mx_turnaround} ({quick_ratio(mx_turnaround, self.T)}%)")
                fmt = fmt + "{:>15s}".format(f"{round(mean_wait_time)} ({quick_ratio(mean_wait_time, self.T)}%)")
            except KeyError:
                fmt = fmt + "no turnaround data" 
            proc_strs.append(fmt)
        summary += "\n".join(proc_strs) + delim
        summary += 'CPU statistics' + nl
        summary += '{0:<4s} - {1:^15s} - {2:^15s} - {3:<15s}'.format('ID','active time','idle time', 'context switch time') + nl
        cpu_strs = []
        for cpu in self.CPUs:
            total = cpu.active_time + cpu.context_switch_time + cpu.idle_time 
            cpu_strs.append(f"{cpu.id:>4d} | " +
                            "{:>15s} | ".format(f"{cpu.active_time} ({quick_ratio(cpu.active_time,total)}%)") +
                            "{:>15s} | ".format(f"{cpu.idle_time} ({quick_ratio(cpu.idle_time,total)}%)") + 
                            "{:>15s}".format(f"{cpu.context_switch_time} ({quick_ratio(cpu.context_switch_time, total)})%"))
        summary += "\n".join(cpu_strs)
        return summary

    def __str__(self):
        return self.summarize()

    def __lt__(self, i):
        return self.T < i

    def __le__(self, i):
        return self.T <= i

    def __gt__(self, i):
        return self.T > i
    
    def __ge__(self, i):
        return self.T >= i
    
    def __len__(self):
        return len(self.EVENT_QUEUE)

def quick_ratio(a,b):
    return f"{round((a/b)*100,2)}"
