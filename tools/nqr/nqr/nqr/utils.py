
# TODO Runs() validate directories
def make_directories(run):
    import os
    os.makedirs(run['dir'], exist_ok=True)

def copy_requirements(run):
    import shutil, os
    for dependency in run['requirements']:
        if dependency.replicate_path:
            dstdir = os.path.join(run['dir'],dependency.parent)
            dstpath = os.path.join(run['dir'],dependency.path)
            os.makedirs(dstdir, exist_ok=True)
            if dependency.isdir:
                shutil.copytree(dependency.path, dstdir, dirs_exist_ok=True)
            else: # dependency.isfile
                shutil.copy2(dependency.path, dstpath)
        else: # do not replicate path
            if dependency.isdir:
                dstdir = os.path.join(run['dir'],dependency.destination)
            else: # isfile
                dstdir = os.path.join(run['dir'],os.path.split(dependency.destination)[0])
            dstpath = os.path.join(run['dir'],dependency.destination)
            os.makedirs(dstdir, exist_ok=True)
            if dependency.isdir:
                shutil.copytree(dependency.path, dstpath, dirs_exist_ok=True)
            else:
                shutil.copy2(dependency.path, dstpath)

