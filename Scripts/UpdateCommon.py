#! /usr/bin/python
#
# Simple script to update a set of common directories that are needed as dependencies of the current project

import os
import shutil
import string
import subprocess
import sys
import urllib
import zipfile
import tarfile
import platform
import xml.etree.ElementTree as xml

MACHINE_OS = ""
if "windows" in platform.system().lower():
    MACHINE_OS = "Windows"
elif "cygwin" in platform.system().lower():
    MACHINE_OS = "Windows"
elif "linux" in platform.system().lower():
    MACHINE_OS = "Linux"
else:
    print("Operating system not recognized correctly")
    sys.exit(1)

# default revision/branch specified in manifest file
defaultRevision = None
# default git server specified in manifest file
defaultServer = None
# list of git servers specified by manifest file
remotes = {}

useShell = False

# RCP git project to folder map definitions
# - GitHubMapping
if MACHINE_OS == "Linux":
    from UpdateCommonMap import downloadMappingLin as downloadMapping
    from UpdateCommonMap import vkStableClocksMappingLin as vkStableClocksMapping
else:
    from UpdateCommonMap import downloadMappingWin as downloadMapping
    from UpdateCommonMap import vkStableClocksMappingWin as vkStableClocksMapping
    useShell = True

# to allow the script to be run from anywhere - not just the cwd - store the absolute path to the script file
scriptRoot = os.path.dirname(os.path.realpath(__file__))

def initializeDefaults(manifest):
    for child in manifest:
        # look for git server settings first
        if child.tag == "remote":
            gitRemoteName = child.get('name')
            gitRemoteFetchStr = child.get('fetch')
            remotes[gitRemoteName] = gitRemoteFetchStr
        if child.tag == "default":
            defaultRevision = child.get('revision')
            defaultServer = child.get('remote')
    return(defaultServer, defaultRevision, remotes)

# updateProject
#    use git pull to update an existing git project folder
# Arguments:
#    path    full path to project's root folder
def updateProject(path):
    # Update .git data
    gitCmd = ["git", "fetch"]
    sys.stdout.flush()
    try:
        subprocess.check_call(gitCmd, cwd=path, shell=useShell)
    except subprocess.CalledProcessError as e:
        print("'git fetch' failed with return code %d\n"%e.returncode)
        sys.stderr.flush()
        sys.exit(e.returncode)
    sys.stdout.flush()
    # Update files
    gitCmd = ["git", "pull", "--no-tags"]
    try:
        subprocess.check_call(gitCmd, cwd=path, shell=useShell)
    except subprocess.CalledProcessError as e:
        print("'git pull' failed with return code %d\n"%e.returncode)
        sys.stderr.flush()
        sys.exit(e.returncode)
    sys.stdout.flush()

# cloneProject
#    use git clone to download a project
# Arguments:
#    sshPath    ssh or http string containing full path to git project upstream or origin
#    targetPath full path to target workspace path
def cloneProject(clonePath, targetPath):
    # Clone clonePath into targetPath
    gitCmd = ['git', 'clone', '--no-tags', clonePath, targetPath]
    sys.stdout.flush()
    try:
        subprocess.check_call(gitCmd, shell=useShell)
    except subprocess.CalledProcessError as e:
        print("'git clone' failed with return code %d\n"%e.returncode)
        sys.stderr.flush()
        sys.exit(e.returncode)
    sys.stdout.flush()

def downloadandunzip(key, value):
    # convert targetPath to OS specific format
    tmppath = os.path.join(scriptRoot, "..", value)
    # clean up path, collapsing any ../ and converting / to \ for Windows
    targetPath = os.path.normpath(tmppath)
    if False == os.path.isdir(targetPath):
        os.makedirs(targetPath)
    zipfileName = key.split('/')[-1].split('#')[0].split('?')[0]
    #zipfileName = os.path.basename(key)
    zipPath = os.path.join(targetPath, zipfileName)
    if False == os.path.isfile(zipPath):
        print("\nDownloading " + key + " into " + zipPath)
        urllib.urlretrieve(key, zipPath)
        if os.path.splitext(zipPath)[1] == ".zip":
            zipfile.ZipFile(zipPath).extractall(targetPath)
        elif os.path.splitext(zipPath)[1] == ".tgz":
            tarfile.open(zipPath).extractall(targetPath)

def handleVkStableClocks(key, value):
    tmppath = os.path.normpath(os.path.join(scriptRoot, "..", value[0]))
    targetPath = os.path.join(tmppath, value[1])
    if False == os.path.isdir(targetPath):
        os.makedirs(targetPath)
    srcPath =  os.path.normpath(os.path.join(tmppath, value[2], key))
    dstPath = os.path.normpath(os.path.join(targetPath, key))
    if False == os.path.isfile(dstPath):
       if True == os.path.isfile(srcPath):
           print("Copying " + srcPath + " to " + dstPath)
           shutil.copy2(srcPath, dstPath)

# for each GitHub dependency - test if it has already been fetched - if not, then fetch it,
# otherwise update it to top of tree

if __name__ == "__main__":
    # Path to RCP project manifest file
    manifestPath = os.path.normpath(os.path.join(scriptRoot, "manifests", "RadeonComputeProfiler.xml"))
    manifestTree = xml.ElementTree(file=manifestPath)
    # initialize xmlElementTree
    manifestRoot = manifestTree.getroot()

    (defaultServer, defaultRevision, remotes) = initializeDefaults(manifestRoot)
    
    for child in manifestRoot:
        if child.tag == "project":
            # determine git server to clone from
            projectName = child.get('name')
            if projectName == "RCP":
                # Skip RCP project, assume already cloned
                continue
                
            clonePrefix = remotes[defaultServer]
            if child.find('remote'):
                clonePrefix = remotes[child.get('remote')] 
                
            # construct clone path and target path
            cloneString = clonePrefix + projectName
            targetPath = os.path.normpath(os.path.join(scriptRoot, "..", "..", child.get('path')))
                                
            # clone or update the project
            projectGitFolder = os.path.join(targetPath, ".git")
            if os.path.isdir(projectGitFolder):
                # project already cloned into workspace, update
                print("Updating %s"%targetPath)
                updateProject(targetPath)
            else:
                # project needs to be cloned into workspace
                print("Cloning %s into %s"%(projectName, targetPath))
                cloneProject(cloneString, targetPath)

    for key in downloadMapping:
        downloadandunzip(key, downloadMapping[key])

    for key in vkStableClocksMapping:
        handleVkStableClocks(key, vkStableClocksMapping[key])
