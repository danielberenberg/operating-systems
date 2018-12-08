"""
RL based preemption
-------------------
The probabilty of choosing a certain action is linked to the amount of reward that 
the agent expects to obtain after taking that action. 

For the case of the Discrete Event Simulator, we would like to minimize process wait times.

The agent will be proceed in the following way:
    - When a process is dispatched, we will assign it a new time quantum
      based on the following exposed features
        - io burst
        - cpu burst
        - time left til I/O burst
        - number of preemptions
        - last quantum assigned
        - cpu demand
        - process type
    - The agent chooses a quantum for that process from its action distribution
    - As a process leaves the system, the agent observes that process' wait time.
    - The process is rewarded in -waitTime, meaning the smallest reward it could
      experience is 0

If a process is generated the take in its state and set its QUANTUM
if a process is dispatched, then take in its state and adjust its QUANTUM 
"""
import random
class AdaptivePreemptor:
    """
    Reinforcement Learning based approach to preemption;
    ---------------------------------------------------
    The AdaptivePreemptor lives inside of a DiscreteEventSimulator;
    its job is to output appropriate time quantums for processes.

    The preemptor will experience each event in which it is necessary to
    assign or update a time quantum. 

    Based on attributes of that event (state), it outputs a quantum.

    The state features that an attribute considers is a vector:

            | T (system_time)
            | demand    
      _     | cpu_current 
      f :=  | last_quantum
            | wait_time
            | io_burst
            | time_in_system
            | idle_time(cpu1, t, t_last)
            |     ... (idle_time for each cpu) 
                                 _                       _
    The preemptor learns weights w = [wi] for each fi in f
    """
    POSSIBLE_ACTIONS = list(range(1,200 + 1))

    def __init__(self,cpu_ct=1,enable_io=True,alpha=0.01, gamma=0.9):
        self.cpu_ct    = cpu_ct
        self.enable_io = enable_io

        self._initialize_features()
        self._initialize_weights()

        self.alpha = alpha
        self.decay = 1e-5
        self.gamma = gamma
        self._t = 0

    def _initialize_features(self):
        self.features = Counter()
        self.features.add_keys(*(f"cpu_{cpu}_idletime" for cpu in range(self.cpu_ct)))
        self.features.add_keys("T",
                               "demand",
                               "cpu_current",
                               "last_quantum",
                               "wait_time",
                               "time_in_system")
        if self.enable_io:
            self.features.add_keys('burst_io')

    def _initialize_weights(self):
        if not hasattr(self,'features'):
            self.initialize_features()
        self.weights = Counter()
        self.weights.add_keys(*(fi for fi in self.features))

        
    def getWeights(self):
        return self.weights

    def getFeatures(self):
        return self.features

    def setTime(self,t):
        self._t = t

    def getQValue(self, state, action):
        """
          Should return Q(state,action) = w * featureVector
          where * is the dotProduct operator
        """
        # initialize running total
        qsa = 0.0    
        
        # terminal state case
        if state is None:
            return qsa
        
        # compute dot product  f(s,a)W, f(s,a) and W each vectors
        for f in self.features:
            qsa += self.getWeights()[f]*state[f]

        return qsa
    
    def update(self, state, action, nextState, reward):
        """
        updates weights based on transition
        args:
            :state     - dictionary last state attributes features
            :action    - last quantum assigned
            :nextState - dictionary of current state attributes 
            :reward    - reward between state and nextState
        """
        
        # 1) get weights
        # 2) compute next max actions
        # 3) compute q(s',a')
        # 4) solve for difference 
        current_wts = self.getWeights()               # 1) 
        max_action = self.getPolicy(nextState)        # 2)
        max_sa = self.getQValue(nextState,max_action) # 3) 
        difference = (reward + self.gamma*max_sa) - self.getQValue(state,action) # 4)
        
        # iterate through features and update weights
        for f in state:
            featValue = state[f] # one feature value
            self.weights[f] = current_wts[f] + self.alpha*difference*featValue # weight update 
        
        for w in self.weights:
            try:
                self.weights[w] = self.weights[w]/sum([self.weights[w] for w in self.weights])
            except ZeroDivisionError:
                pass
        self.alpha -= self.decay

    def getPolicy(self, state):
        if random.random() > 0.01:
            return random.choice(AdaptivePreemptor.POSSIBLE_ACTIONS)
        return self.computeActionFromQValues(state)

    def computeActionFromQValues(self, state):
        """
          Compute the best action to take in a state.  Note that if there
          are no legal actions, which is the case at the terminal state,
          you should return None.
        """
        maxAction       = None
        maxValue        = -1*float('inf')
        
        # terminal state case
        if len(AdaptivePreemptor.POSSIBLE_ACTIONS) == 0:
            return maxAction
        
        else:
            # compare each legal action to the stored maximum by comparing
            # action values and storing both the top action and its value
            for a in AdaptivePreemptor.POSSIBLE_ACTIONS:
                if self.getQValue(state,a) >= maxValue:
                    maxValue = self.getQValue(state,a)
                    maxAction = a
        return maxAction

def sign( x ):
    """
    Taken from BerkeleyAI course:
    [http://ai.berkeley.edu/reinforcement.html#Q8]

    Returns 1 or -1 depending on the sign of x
    """
    if( x >= 0 ):
        return 1
    else:
        return -1

class Counter(dict):
    """
    Taken from BerkeleyAI course:
    [http://ai.berkeley.edu/reinforcement.html#Q8]

    A counter keeps track of counts for a set of keys.

    The counter class is an extension of the standard python
    dictionary type.  It is specialized to have number values
    (integers or floats), and includes a handful of additional
    functions to ease the task of counting data.  In particular,
    all keys are defaulted to have value 0.  Using a dictionary:

    a = {}
    print a['test']

    would give an error, while the Counter class analogue:

    >>> a = Counter()
    >>> print a['test']
    0

    returns the default 0 value. Note that to reference a key
    that you know is contained in the counter,
    you can still use the dictionary syntax:

    >>> a = Counter()
    >>> a['test'] = 2
    >>> print a['test']
    2

    This is very useful for counting things without initializing their counts,
    see for example:

    >>> a['blah'] += 1
    >>> print a['blah']
    1

    The counter also includes additional functionality useful in implementing
    the classifiers for this assignment.  Two counters can be added,
    subtracted or multiplied together.  See below for details.  They can
    also be normalized and their total count and arg max can be extracted.
    """
    def add_keys(self, *keys):
        for k in keys:
            self.setdefault(k, 0)
            
    def __getitem__(self, idx):
        self.setdefault(idx, 0)
        return dict.__getitem__(self, idx)

    def incrementAll(self, keys, count):
        """
        Increments all elements of keys by the same count.

        >>> a = Counter()
        >>> a.incrementAll(['one','two', 'three'], 1)
        >>> a['one']
        1
        >>> a['two']
        1
        """
        for key in keys:
            self[key] += count

    def argMax(self):
        """
        Returns the key with the highest value.
        """
        if len(self.keys()) == 0:
            return None
        all_ = self.items()
        values = [x[1] for x in all]
        maxIndex = values.index(max(values))
        return all_[maxIndex][0]

    def sortedKeys(self):
        """
        Returns a list of keys sorted by their values.  Keys
        with the highest values will appear first.

        >>> a = Counter()
        >>> a['first'] = -2
        >>> a['second'] = 4
        >>> a['third'] = 1
        >>> a.sortedKeys()
        ['second', 'third', 'first']
        """
        sortedItems = self.items()
        sortedItems.sort(cmp=lambda x,y:sign(y[1] - x[1]))
        return [x[0] for x in sortedItems]

    def totalCount(self):
        """
        Returns the sum of counts for all keys.
        """
        return sum(self.values())

    def normalize(self):
        """
        Edits the counter such that the total count of all
        keys sums to 1.  The ratio of counts for all keys
        will remain the same. Note that normalizing an empty
        Counter will result in an error.
        """
        total = float(self.totalCount())
        if total == 0:
            return
        for key in self.keys():
            self[key] = self[key] / total

    def divideAll(self, divisor):
        """
        Divides all counts by divisor
        """
        divisor = float(divisor)
        for key in self:
            self[key] /= divisor

    def copy(self):
        """
        Returns a copy of the counter
        """
        return Counter(dict.copy(self))

    def __mul__(self, y ):
        """
        Multiplying two counters gives the dot product of their vectors where
        each unique label is a vector element.

        >>> a = Counter()
        >>> b = Counter()
        >>> a['first'] = -2
        >>> a['second'] = 4
        >>> b['first'] = 3
        >>> b['second'] = 5
        >>> a['third'] = 1.5
        >>> a['fourth'] = 2.5
        >>> a * b
        14
        """
        sum = 0
        x = self
        if len(x) > len(y):
            x,y = y,x
        for key in x:
            if key not in y:
                continue
            sum += x[key] * y[key]
        return sum

    def __radd__(self, y):
        """
        Adding another counter to a counter increments the current counter
        by the values stored in the second counter.

        >>> a = Counter()
        >>> b = Counter()
        >>> a['first'] = -2
        >>> a['second'] = 4
        >>> b['first'] = 3
        >>> b['third'] = 1
        >>> a += b
        >>> a['first']
        1
        """
        for key, value in y.items():
            self[key] += value

    def __add__( self, y ):
        """
        Adding two counters gives a counter with the union of all keys and
        counts of the second added to counts of the first.

        >>> a = Counter()
        >>> b = Counter()
        >>> a['first'] = -2
        >>> a['second'] = 4
        >>> b['first'] = 3
        >>> b['third'] = 1
        >>> (a + b)['first']
        1
        """
        addend = Counter()
        for key in self:
            if key in y:
                addend[key] = self[key] + y[key]
            else:
                addend[key] = self[key]
        for key in y:
            if key in self:
                continue
            addend[key] = y[key]
        return addend

    def __sub__( self, y ):
        """
        Subtracting a counter from another gives a counter with the union of all keys and
        counts of the second subtracted from counts of the first.

        >>> a = Counter()
        >>> b = Counter()
        >>> a['first'] = -2
        >>> a['second'] = 4
        >>> b['first'] = 3
        >>> b['third'] = 1
        >>> (a - b)['first']
        -5
        """
        addend = Counter()
        for key in self:
            if key in y:
                addend[key] = self[key] - y[key]
            else:
                addend[key] = self[key]
        for key in y:
            if key in self:
                continue
            addend[key] = -1 * y[key]
        return addend

