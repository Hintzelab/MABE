from collections import defaultdict
from math import log2,ceil
import numpy as np

## Run this script by itself to see the demo work,
## otherwise import it and use the R() function:
## from neurocorrelates import R

def main():
  print("running demo, calculating R:")
  states = [[[0,0,1,0,1,0,1],[0,1,1,0,1,1],[0,0,1,1]],
            [[1,1,1,1,0,1,0],[1,0,1,1,0,0],[1,1,0,0]],
            [[0,0,1,0,1,0,1],[1,0,1,1,0,0],[0,1,0,1]]]
  states = np.array(states)
  print( R(states) )
  print('ran okay')

# shared entropy (information) between the brain and the environment not shared with the sensors  
# R = H(S,E) + H(S,M) - H(S) - H(E,M,S)
def R(state_time_series,progress_width=0):
  ## assumes a time(t) state is [[sensor],[environment],[memory]]
  ## so [[0,1,0,0],[1,0,0,0,1,1],[0,1]] is one timeslice
  sensor_observations = defaultdict(int)
  environment_sensor_observations = defaultdict(int)
  memory_sensor_observations = defaultdict(int)
  total_observations = defaultdict(int)

  resolution = 1.0 / len(state_time_series)
  t = 0
  maxt = len(state_time_series)
  screen_progress = 0

  for (sensor_state, environment_state, memory_state) in state_time_series:
    sensor_observations[ concat_bit_lists(sensor_state) ] += 1
    environment_sensor_observations[ concat_bit_lists(sensor_state,environment_state) ] += 1
    memory_sensor_observations[ concat_bit_lists(sensor_state,memory_state) ] += 1
    total_observations[ concat_bit_lists(sensor_state,environment_state,memory_state) ] += 1
    if progress_width:
      screen_progress = ceil((float(t)/float(maxt)) * progress_width)
      t += 1
      print('['+('.'*screen_progress)+' '*(progress_width-screen_progress)+']',end='\r')
  if progress_width:
    screen_progress = ceil((float(t)/float(maxt)) * progress_width)
    print('['+('.'*screen_progress)+' '*(progress_width-screen_progress)+']',end='\n') ## (\n)ewline
  # R = H(S,E) + H(S,M) - H(S) - H(E,M,S)
  H_SE = calcEntropy(environment_sensor_observations, resolution)
  H_SM = calcEntropy(memory_sensor_observations, resolution)
  H_S = calcEntropy(sensor_observations, resolution)
  H_EMS = calcEntropy(total_observations, resolution)
  return H_SE + H_SM - H_S - H_EMS

def calcEntropy(observations_map, resolution):
  ## observations_list: map[int,int]
  ## resolution: float
  entropy_summation = 0.0
  p = 0.0
  for count in observations_map.values():
    p = resolution * count
    entropy_summation += p * log2(p)
  return -1 * entropy_summation

def concat_bit_lists(*bitlists):
  ## mask is a bitmask selecting the associated nums of the list
  ## then those selected nums are bit-wise concatenated
  numitems = len(bitlists)
  newint = 0
  # loop through all bits of the bitmask
  for each_bitlist in bitlists:
    for each_bit in each_bitlist:
      newint = (newint << 1) + each_bit
  return newint

if __name__ == '__main__':
  main()
