import os
import sys
import imp # module dependency checking
import subprocess # invoking command line module installation

suitableModuleInstallers = "conda,pip3,pip".split(',')
preferredInstaller='' ## probably conda, but might be pip
alternateInstaller='' ## probably pip
installCMDString = ''
modulesAreMissing = False

def testForModuleAndBuildInstallString(moduleName):
    global installCMDString, modulesAreMissing
    ## the split(':') allows there to exist
    ## strings like "bzip2:bz2" meaning "installName:importName"
    ## when there is a difference. If they are the same, then
    ## "colorama" without the ':' will suffice.
    try:
        imp.find_module(moduleName.split(':')[-1])
    except ImportError:
        installCMDString += moduleName.split(':')[0]+' '
        modulesAreMissing = True

def executableExistsInPath(exename,separator):
    foundPath = None
    for extension in ['','.exe']:
        if foundPath is not None:
            break
        for path in os.environ["PATH"].split(os.pathsep):
            if foundPath is not None:
                break
            fp = os.path.join(path,exename).replace(os.pathsep,separator)+extension
            if os.path.isfile(fp): #and os.access(os.path.join(path,exename),os.X_OK):
                foundPath = fp
    return foundPath

def reportPath():
    print(os.environ["PATH"].split(os.pathsep))

def reportInstaller():
    global preferredInstaller, alternateInstaller
    for eachInstaller in suitableModuleInstallers:
        installerPath = executableExistsInPath(eachInstaller,'/') # handles *nix and *nix-on-win
        if installerPath is not None:
            if preferredInstaller == '':
                preferredInstaller = eachInstaller
                continue
            else:
                alternateInstaller = eachInstaller
                break
        installerPath = executableExistsInPath(eachInstaller,'\\') # handles running in standard win
        if installerPath is not None:
            if preferredInstaller == '':
                preferredInstaller = eachInstaller
                continue
            else:
                alternateInstaller = eachInstaller
                break
    if preferredInstaller == '':
        print("Error: no suitable python installer found of either "+', '.join(suitableModuleInstallers))
        print("       Please run the following command using your python module installer:")
        print("       "+preferredInstaller + ' install '+installCMDString)
        sys.exit(1)
    #print("Found module installer "+preferredInstaller)
    #print("Found alternate installer "+alternateInstaller)

def require(names): ## the only exported fn
    global installCMDString, modulesAreMissing, preferredInstaller, alternateInstaller, suitableModuleInstallers
    userFlag = '--user '
    '''names = a comma separated string of module names, no spaces.
    Can handle packages where the install name is not the same as the import name
    "packagename,packagename,..."
    "packagename,packagename:importname,packagename:importname,..."
    example: pyreq.require("colorama,psutil")
    example: pyreq.require("winreg,beautifulsoup4:bs4")'''

    requiredNonstandardModules = names.split(',')
    requiredNonstandardModules = set(requiredNonstandardModules) ## remove dups
    for moduleName in requiredNonstandardModules:
        testForModuleAndBuildInstallString(moduleName)
    if modulesAreMissing:
        print("Error: Required python modules are missing: {modules}".format(modules=', '.join(installCMDString.split())))
        print()
        response = 'NoInput'
        while response not in ['','y','n','yes','no']:
            response = input("Should I try to install them? [Y/n] (Enter defaults yes): ").lower()
        if response in ['','y','yes']:
            preferredInstaller = ''
            for eachInstaller in suitableModuleInstallers:
                installerPath = executableExistsInPath(eachInstaller,'/') # handles *nix and *nix-on-win
                if installerPath is not None:
                    if preferredInstaller == '':
                        preferredInstaller = eachInstaller
                        continue
                    else:
                        alternateInstaller = eachInstaller
                        break
                installerPath = executableExistsInPath(eachInstaller,'\\') # handles running on standard win
                if installerPath is not None:
                    if preferredInstaller == '':
                        preferredInstaller = eachInstaller
                        continue
                    else:
                        alternateInstaller = eachInstaller
                        break
            if preferredInstaller is None:
                print("Error: no suitable python installer found of either "+', '.join(suitableModuleInstallers))
                print("       Please run the following command using your python module installer:")
                print("       "+preferredInstaller + ' install '+installCMDString)
                sys.exit(1)
            if preferredInstaller == "conda":
                userFlag = "" ## don't use '--user' if using conda
            print("Found module installer "+preferredInstaller)
            print("Found alternate installer "+alternateInstaller)
            try:
                subprocess.run(preferredInstaller + ' install '+userFlag+installCMDString, shell=True, check=True)
            except subprocess.CalledProcessError:
                print("Module not found using '"+preferredInstaller+"', trying '"+alternateInstaller+"'...")
                try:
                    subprocess.run(alternateInstaller + ' install '+userFlag+installCMDString, shell=True, check=True)
                except subprocess.CalledProcessError:
                    print("Error: module installation using the following installation system failed:")
                    print("       "+preferredInstaller)
                    sys.exit(1)
            print()
            print("No obvious errors I could detect. Please try to run the script as you were again (press the up arrow on your keyboard to recall previous commands).")
        sys.exit(0)

def tests():
    pass

if __name__ == '__main__':
    tests()
