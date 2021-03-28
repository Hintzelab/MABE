# * make requirements.txt from pip freeze info (parsimonious + python are the only things?)
# * what else makes a package?
# * make sure can 'pip install -e nqr'... something?
# * try it out on the hpcc!

## TODO this is a hack to get relative imports working
##      until we can make this a proper installable module
import os, sys
#PACKAGE_PARENT = '..'
#SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
#sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))
## END HACK

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
include('../tools')
import mabe

# read or set parameters
#mabe.read("settings*.cfg") # overwrites namespaces if conflict
#mabe.GLOBAL.updates = 0

# experiment variables
with Group("UPDATES"):
    Vary(mabe.GLOBAL.updates, [1000,2000]) # name optional

mabe.setreps(1) # or mabe.reps(range(101,400))

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
