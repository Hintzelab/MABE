import os, sys

###################
# TEST GENERIC MQ #
###################
#import nqr
#from nqr import generic as g, Vary, Group
#
#g.GLOBAL = nqr.NS()
#g.GLOBAL.worldType = 'Berry'
#g.GLOBAL.SUBGLOBAL = nqr.NS()
#g.GLOBAL.SUBGLOBAL.garsnutch = 3.14
#
#with Group():
#    Vary(g.GLOBAL.worldType, list(range(2)), name='WT' ) # name optional
#    #Vary(g.GLOBAL.worldType, ['a','b d'], labels=['ahh','bahh'], name='WT' ) # name optional
#
## simple system command (does not use Vary system)
##g.addcommand('echo "dir: $(pwd)"')
## parameterized system command
##g.addcommand(exe='echo', template='{exe} "id-{id}: $(pwd)"')
## simple custom executable command
##g.addcommand('mabe')
## parameterized custom executable command
#g.addcommand(exe='mabe',template='{exe} -p GLOBAL-randomSeed {rep} -p GLOBAL-updates 999')
## parameterized custom analysis script
##g.addcommand(exe='python analysis.py',template='{exe} -r {rep} {parameters}')
#
#g.requires('mabe')
#g.setreps(1)
#
#from nqr.targets import local
#for run in g.Runs(priority=g.CONDITIONS):
#    local.launch(run)

################
# TEST MABE MQ #
################
#import nqr
#from nqr import Vary, Group, include
import nqr
from nqr import Vary, Group, include

# this include() assumes we're in the work dir
include('../tools')
import mabe

# read or set parameters
#mabe.read("settings*.cfg") # overwrites namespaces if conflict
mabe.GLOBAL.updates = 100

# experiment variables
with Group():
    Vary(mabe.GLOBAL.updates, [5,10]) # name optional
with Group():
    Vary(mabe.WORLD_TEST.numberOfOutputs, [5,10]) # name optional

mabe.setreps(10) # or mabe.reps(range(101,400))

mabe.requires('mabe')

mabe.addcommand('module load GCC/10.2.0')
mabe.addcommand('mabe','{exe} -p GLOBAL-randomSeed {rep} {parameters}', output=['*.csv'])

from nqr.targets import local, slurm, targets

slurm.settings.ntasks = 1
slurm.settings.constraint = "[amd20]"
slurm.settings.time = "00:05:00"
slurm.settings.mail_type = "FAIL"
slurm.settings.mem = "50M"

for run in mabe.Runs():
    #slurm.launch(run)
    #local.launch(run)
    targets.launch(run)
