#!/usr/bin/env python

import os

def getFilesRecursively(directory, extension = ".ts"):
    """
    Returns the files recursively in a directory matching a pattern.
    """
    fileList = []
    for root, dirs, files in os.walk(directory, topdown=True):
        for filename in files:
            fullpath = os.path.join(root, filename)
            if os.path.isfile(fullpath) and filename.endswith(extension):
                fileList.append(fullpath)
    return fileList

def filesAreEqual(file1, file2):
    """
    Compare the files file1 and file2 and returns wheter or not they are equal.
    """
    import filecmp
    return filecmp.cmp(file1, file2)

def compareFiles(currentDir, previousDir, fileList):
    """
    Checks which files were changed or added compared to the previous version.
    """
    result = {"ChangedFiles" : [], "NewFiles" : []}
    for file in fileList:
        # ignore already updated files
        if file.endswith("_done.ts"):
            continue
        # check if file has been changed
        prevFullpathOrig = file.replace(currentDir, previousDir)
        prevFullpathDone = prevFullpathOrig.replace(".ts", "_done.ts")
        fileFound = False
        for prevFileFullPath in [prevFullpathOrig, prevFullpathDone]:
            if os.path.isfile(prevFileFullPath):
                fileFound = True
                if not filesAreEqual(file, prevFileFullPath):
                    result["ChangedFiles"].append(file)
        # check if previous file exists
        if not fileFound:
            result["NewFiles"].append(file)
    return result

def fileComparisonToStr(fileComparisonDict):
    indent = ' ' * 6
    result = "Found %d changed files:\n" % \
             len(fileComparisonDict["ChangedFiles"])
    for file in fileComparisonDict["ChangedFiles"]:
        result += "%s|- %s\n" % (indent, file)
    result += "Found %d new files:\n" % \
              len(fileComparisonDict["NewFiles"])
    for file in fileComparisonDict["NewFiles"]:
        result += "%s|- %s\n" % (indent, file)
    return result

def main():
    # Versions to compare
    current, previous = "3.2.0-alpha7", "3.1-beta-6"
    current, previous = "3.2.0-beta.2", "3.2.0-alpha7"
    # Dictionary mapping from BabylonJs version to relative path
    BabylonJsVersions = {
        "3.1-alpha" : "3.1.0_2017_09_23",
        "3.1-beta-6" : "3.1.0_2017_12_01",
        "3.2.0-alpha7" : "3.2.0_2018_02_03",
        "3.2.0-beta.2" : "3.2.0_2018_03_22"
    }
    # Create mapping from BabylonJs version to full path
    for version in BabylonJsVersions:
        fullPath = os.path.join(os.path.expanduser('~'), "Projects",
                                "Babylon.js-%s" % BabylonJsVersions[version],
                                "src")
        BabylonJsVersions[version] = fullPath
    # Perform comparison
    previous = BabylonJsVersions[previous]
    current = BabylonJsVersions[current]
    files = getFilesRecursively(current)
    fileComparisonDict = compareFiles(current, previous, files)
    print fileComparisonToStr(fileComparisonDict)

if __name__ == "__main__":
    main()
