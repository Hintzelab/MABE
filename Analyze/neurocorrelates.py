from collections import defaultdict
from math import log2,ceil
import numpy as np

## Run this script by itself to see the demo work,
## otherwise import it and use the R() function:
## from neurocorrelates import R

def main():
  print("running demo, calculating R:")
  #          inputs          world     memory
  #          v               v         v
  #          0 1 2 3 4 5 6 7 0 1 2 3 4 0 1 2 3
  states = [[0,0,1,0,1,0,1,0,1,1,0,1,1,0,0,1,1],
            [1,1,1,1,0,1,0,1,0,1,1,0,0,1,1,0,0],
            [0,0,1,0,1,0,1,1,0,1,1,0,0,0,1,0,1]]
  states = np.array(states)
  # now mask 'on' the bits you care about when calling R()
  #        states   inputs           world   memory    numInputBits    numWorldBits
  print( R(states,  list(range(7)),  (0,2),  (0,2,3),  n_senBits=7,    n_envBits=6) )
  print('ran okay')

# shared entropy (information) between the brain and the environment not shared with the sensors  
# R = H(S,E) + H(S,M) - H(S) - H(E,M,S)
def R(state_time_series, sensorBitMask, environmentBitMask, memoryBitMask, n_senBits, n_envBits, progress_width=0):
  ## assumes a time(t) state is [[sensor],[environment],[memory]]
  ## so [[0,1,0,0],[1,0,0,0,1,1],[0,1]] is one timeslice
  sensor_observations = defaultdict(int)
  sensor_environment_observations = defaultdict(int)
  sensor_memory_observations = defaultdict(int)
  total_observations = defaultdict(int)

  resolution = 1.0 / len(state_time_series)
  t = 0
  maxt = len(state_time_series)
  screen_progress = 0

  sensorBitMask = tuple(np.array(sensorBitMask))
  environmentBitMask = tuple(np.array(environmentBitMask)+n_senBits)
  memoryBitMask = tuple(np.array(memoryBitMask)+n_senBits+n_envBits)

  sensorBits = state_time_series[:,sensorBitMask]
  environmentBits = state_time_series[:,environmentBitMask]
  memoryBits = state_time_series[:,memoryBitMask]

  sensorInts = bitsToInts(sensorBits)
  sensorAndEnvironmentInts = bitsToInts(np.concatenate((sensorBits,environmentBits),axis=1))
  sensorAndMemoryInts = bitsToInts(np.concatenate((sensorBits,memoryBits),axis=1))
  totalInts = bitsToInts(np.concatenate((sensorBits,environmentBits,memoryBits),axis=1))

  for (S,SE,SM,SEM) in zip(sensorInts,sensorAndEnvironmentInts,sensorAndMemoryInts,totalInts):
    sensor_observations[S] += 1
    sensor_environment_observations[SE] += 1
    sensor_memory_observations[SM] += 1
    total_observations[SEM] += 1
  if progress_width:
    screen_progress = ceil((float(t)/float(maxt)) * progress_width)
    print('['+('.'*screen_progress)+' '*(progress_width-screen_progress)+']',end='\n') ## (\n)ewline
  ## R = H(S,E) + H(S,M) - H(S) - H(E,M,S)
  H_SE = calcEntropy(sensor_environment_observations, resolution)
  H_SM = calcEntropy(sensor_memory_observations, resolution)
  H_S = calcEntropy(sensor_observations, resolution)
  H_SEM = calcEntropy(total_observations, resolution)
  return H_SE + H_SM - H_S - H_SEM

def bitsToInts(d):
  import numpy as np
  import math
  '''d can be numpy 2-d array, or 1-d array'''
  if len(d.shape) == 1:
    # assume d is 1-dimensional
    multiple_of_8 = math.ceil(d.size/8)
    pad_width = multiple_of_8*8 - d.size
    u8ints = np.pad(d, (pad_width,0), 'constant')
    num = 0
    for n in np.packbits(u8ints):
      num = np.bitwise_or(np.left_shift(num,8),n)
    return num
  # assume d is 2-dimensional
  multiple_of_8 = math.ceil(d[0].size/8)
  pad_width = multiple_of_8*8 - d[0].size
  fullbits = np.pad(d, ((0,0),(pad_width,0)), 'constant')
  ints = np.packbits(fullbits,axis=1)
  nums = np.zeros((len(d),1),dtype=np.int64)
  for i in range(ints[0].size):
    nums = np.bitwise_or(np.left_shift(nums,8),ints[:,i].reshape(len(ints),1))
  return nums.reshape(len(ints))

def calcEntropy(observations_map, resolution):
  ## observations_list: map[int,int]
  ## resolution: float
  entropy_summation = 0.0
  p = 0.0
  for count in observations_map.values():
    p = resolution * count
    entropy_summation += p * log2(p)
  return -1 * entropy_summation

def _testBitsToInts():
    import numpy as np
    a = np.array([[1,0,0,0,0,0,1,1,0],[1,0,0,0,1,0,0,0,1]])
    print("testing with vec =",a)
    print(bitsToInts(a))

def _testR():
    import numpy as np
    import numpy as np
    a = np.array([[1,0,0,0,0,0,1,1,0],[1,0,0,0,1,0,0,0,1]])
    print("testing with vec =",a)
    res = R( state_time_series=a, sensorBitMask=list(range(3)), environmentBitMask=list(range(3)), memoryBitMask=list(range(3)) )
    print(res)

if __name__ == '__main__':
  #_testBitsToInts()
  #_testR()
  main()
