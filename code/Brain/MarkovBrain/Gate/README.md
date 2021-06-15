## Markov Brain Gates
This is an overview of gates avalible in markov brains. Short descriptions are provided for some of the gates in a haphazared manner.
for a desciption of markov brains see: https://github.com/Hintzelab/MABE/wiki/Brain-Markov


## ComparatorGate
this gate is capable of comparing doubles. ComparatorGates have two input and one output.
ComparatorGates have 2 modes

in mode 1 the input 0 is a program bit and input 1 is a value to either store (when program bit true) or compare to stored values.
output is 0 if progrma input is on or the input value does not equal the stored value, output is 1 if input value matches the stored value

in mode 2 the inputs 0 and 1 are compared directly. output is 1 if the values are the same

## DecomposableDirectGate


## DecomposableFeedbackGate


## DecomposableGate


## DeterministicGate
takes 1 to 4 inputs and generates 1 to 4 outputs (these ranges may be changed with parameters).
inputs are converted to bits ( 0 if <= 0, 1 if > 0)
output values are binary and generated via a lookup table


## EpsilonGate
Like DeterministicGate, but with error
epsilon is the likelyhood of error

## FeedbackGate
Like probablistic, but includes 2 additional inputs which allow feedback to the gate to either up or down regulate probablities in the lookup table

## GPGate
GPGates preform mathimatical operations on inputs in order to generate outputs

## IzhikevichGate
based on work by: Eugene M. Izhikevich and described in "Simple Model of Spiking Neurons"
published in IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 14, NO. 6, NOVEMBER 2003

## NeuronGate
See: https://github.com/Hintzelab/MABE/wiki/Gate-Neuron
displayNeuronBehavior.py

## PassthroughGate
Takes a single input and outputs the same value (this gate can move values from T to T+1)

## ProbabilisticGate
Like DeterministicGates, except that the lookup table is probablistic

## TritDeterministicGate
Like DeterministicGates, except that inputs and outputs incude -1
inputs are converted as less then 0 = -1, 0 = 0, and greater then 0 = 1

## VoidGate
Like DeterministicGates, except that these is a chance (epsilon) that some output will be voided (i.e. set to 0) 
