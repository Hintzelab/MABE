import subprocess
import os
import argparse

print(subprocess.check_output("pwd"))
output = subprocess.check_output(["./mabe","-f", "settings*", "|", "grep", '"optimizeValue_0"'], shell=True)

print(output)