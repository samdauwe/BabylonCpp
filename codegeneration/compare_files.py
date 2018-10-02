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

def compareFiles(currentDir, previousDir, fileList, whiteList):
    """
    Checks which files were changed or added compared to the previous version.
    """
    result = {"ChangedFiles" : [], "NewFiles" : []}
    for file in fileList:
        # ignore already updated files
        if file.endswith("_done.ts") or file.endswith(tuple(whiteList)):
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
    current, previous = "3.3.0-alpha.9", "3.3.0-alpha.2"
    current, previous = "3.3.0-alpha.13", "3.3.0-alpha.9"
    current, previous = "3.3.0-beta.3", "3.3.0-alpha.13"
    current, previous = "3.3.0", "3.3.0-beta.3"
    # Dictionary mapping from BabylonJs version to relative path
    BabylonJsVersions = {
        "3.3.0-alpha.2" : "3.3.0_2018_05_24",
        "3.3.0-alpha.9" : "3.3.0_2018_06_24",
        "3.3.0-alpha.13": "3.3.0_2018_07_24",
        "3.3.0-beta.3"  : "3.3.0_2018_08_24",
        "3.3.0"         : "3.3.0_2018_10_02",
    }
    # List containing the files to ignore
    whiteList = ["babylon.nullEngine.ts", "babylon.khronosTextureContainer.ts",
                 "babylon.assetsManager.ts", "babylon.virtualJoystick.ts",
                 "babylon.decorators.ts", "babylon.andOrNotEvaluator.ts",
                 "babylon.dracoCompression.ts", "babylon.vrExperienceHelper.ts",
                 "babylon.webVRCamera.ts", "babylon.analyser.ts",
                 "babylon.stereoscopicCameras.ts", "babylon.sceneSerializer.ts",
                 "babylon.videoTexture.ts", "babylon.database.ts",
                 "babylon.promise.ts", "babylon.videoDome.ts", "babylon.tga.ts",
                 "babylon.sound.ts", "babylon.environmentTextureTools.ts",
                 "babylon.arcRotateCameraVRDeviceOrientationInput.ts",
                 "babylon.vrDeviceOrientationGamepadCamera.ts",
                 "babylon.vrDeviceOrientationFreeCamera.ts", "babylon.webvr.ts",
                 "babylon.vrDeviceOrientationArcRotateCamera.ts",
                 "babylon.internalTextureLoader.ts",
                 "babylon.ddsTextureLoader.ts", "babylon.ktxTextureLoader.ts",
                 "babylon.tgaTextureLoader.ts", "babylon.envTextureLoader.ts",
                 "babylon.babylonFileLoader.ts", "babylon.videoRecorder.ts"]
    # Create mapping from BabylonJs version to full path
    for version in BabylonJsVersions:
        modules = {
          "Babylonjs": os.path.join(os.path.expanduser('~'), "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "src"),
          "MaterialsLibrary": os.path.join(os.path.expanduser('~'), "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "materialsLibrary", "src"),
        }
        BabylonJsVersions[version] = modules
    # Perform comparison
    for key, val in BabylonJsVersions[current].items():
      print("### %s ###" % key)
      prev = BabylonJsVersions[previous][key]
      curr = BabylonJsVersions[current][key]
      files = getFilesRecursively(curr)
      fileComparisonDict = compareFiles(curr, prev, files, whiteList)
      print(fileComparisonToStr(fileComparisonDict))

if __name__ == "__main__":
    main()
