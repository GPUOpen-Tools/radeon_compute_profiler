#! /usr/bin/python
#
# Simple script to update a set of common directories that are needed as dependencies of the current project

import os
import string
import subprocess
import sys
import urllib
import zipfile
import tarfile
import platform

MACHINE_OS = ""
if "windows" in platform.system().lower():
    MACHINE_OS = "Windows"
elif "linux" in platform.system().lower():
    MACHINE_OS = "Linux"
else:
    print("Operating system not recognized correctly")
    sys.exit(1)

# RCP git project to folder map definitions
# - GitHubMapping
from UpdateCommonMap import *
if MACHINE_OS == "Linux":
    from UpdateCommonMap import downloadMappingLin as downloadMapping
else:
    from UpdateCommonMap import downloadMappingWin as downloadMapping

# to allow the script to be run from anywhere - not just the cwd - store the absolute path to the script file
scriptRoot = os.path.dirname(os.path.realpath(__file__))

# for each GitHub dependency - test if it has already been fetched - if not, then fetch it,
# otherwise update it to top of tree

for key in GitHubMapping:
    # convert targetPath to OS specific format
    tmppath = os.path.join(scriptRoot, "..", GitHubMapping[key][0])
    # clean up path, collapsing any ../ and converting / to \ for Windows
    targetPath = os.path.normpath(tmppath)
    if os.path.isdir(targetPath):
        print("\nDirectory " + targetPath + " exists, using 'git pull' to get latest")
        p = subprocess.Popen(["git","pull"], cwd=targetPath)
        p.wait();
    else:
        print("\nDirectory " + targetPath + " does not exist, using 'git clone' to get latest")
        gitamdRoot = "https://github.com/GPUOpen-Tools/" + key
        commandArgs = ["git", "clone", gitamdRoot, targetPath]
        p = subprocess.Popen( commandArgs )
        p.wait()
        p = subprocess.Popen(["git","reset","--hard",GitHubMapping[key][1]], cwd=targetPath)
        p.wait()

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

for key in downloadMapping:
    downloadandunzip(key, downloadMapping[key])
