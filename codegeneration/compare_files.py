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
    current, previous = "4.0.0", "3.3.0"
    current, previous = "4.1.0-alpha.17", "4.0.0"
    current, previous = "4.1.0-beta.13", "4.1.0-alpha.17"
    # Dictionary mapping from BabylonJs version to relative path
    BabylonJsVersions = {
        "3.3.0"          : "3.3.0_2018_10_02",
        "4.0.0"          : "4.0.0_2019_04_30",
        "4.1.0-alpha.17" : "4.1.0-alpha.17_2019_08_30",
        "4.1.0-beta.13"   : "4.1.0-beta.13_2019_12_13"
    }
    # List containing the files to ignore
    whiteList = [
        # Common
        "index.ts",
        # Audio
        "analyser.ts", "audioEngine.ts", "audioSceneComponent.ts", "sound.ts",
        "soundTrack.ts", "weightedsound.ts",
        # Cameras / Inputs
        "arcRotateCameraVRDeviceOrientationInput.ts",
        "freeCameraDeviceOrientationInput.ts",
        "freeCameraVirtualJoystickInput.ts",
        # Cameras / RigModes
        "vrRigMode.ts", "webVRRigMode.ts",
        # Cameras / VR
        "vrCameraMetrics.ts", "vrDeviceOrientationArcRotateCamera.ts",
        "vrDeviceOrientationFreeCamera.ts", "vrExperienceHelper.ts",
        "vrDeviceOrientationGamepadCamera.ts", "webVRCamera.ts",
        # Cameras / XR
        "webXRCamera.ts", "webXRController.ts", "webXRControllerModelLoader.ts",
        "webXRControllerPointerSelection.ts", "webXRControllerTeleportation.ts",
        "webXRDefaultExperience.ts", "webXREnterExitUI.ts",
        "webXRExperienceHelper.ts", "webXRFeaturesManager.ts", "webXRInput.ts",
        "webXRManagedOutputCanvas.ts","webXRSessionManager.ts", "webXRTypes.ts",
        # Cameras / XR / features
        "WebXRAnchorSystem.ts", "WebXRBackgroundRemover.ts",
        "WebXRHitTestLegacy.ts", "WebXRPlaneDetector.ts",
        # Debug
        "debugLayer.ts",
        # Engines
        "nativeEngine.ts",
        # Engines / Extensions
        "engine.webVR.ts",
        # Engines / Native
        "nativeShaderProcessor.ts",
        # Helpers
        "videoDome.ts",
        # LibDeclarations
        "browser.d.ts", "webgl.d.ts", "webgl2.d.ts", "webvr.d.ts", "webxr.d.ts",
        # Loading
        "loadingScreen.ts",
        # Materials / Textures
        "basisTextureLoader.ts", "htmlElementTexture.ts", "videoTexture.ts",
        # Meshes / Compression
        "dracoCompression.ts",
        # Misc
        "andOrNotEvaluator.ts", "assetsManager.ts", "basis.ts", "baseError.ts",
        "canvasGenerator.ts", "customAnimationFrameRequester.ts",
        "decorators.ts", "deepCopier.ts", "deferred.ts", "domManagement.ts",
        "fileRequest.ts", "filesInput.ts", "filesInputStore.ts",
        "instantiationTools.ts", "logger.ts", "loadFileError.ts",
        "precisionDate.ts", "promise.ts", "retryStrategy.ts",
        "sceneSerializer.ts", "screenshotTools.ts", "smartArray.ts",
        "stringDictionary.ts", "tags.ts", "typeStore.ts", "videoRecorder.ts",
        "virtualJoystick.ts", "webRequest.ts", "workerPool.ts",
        # Offline
        "database.ts", "IOfflineProvider.ts",
        # Physics / Plugins
        "ammoJSPlugin.ts", "cannonJSPlugin.ts", "oimoJSPlugin.ts"
    ]
    # Create mapping from BabylonJS version to full path
    for version in BabylonJsVersions:
        modules = {
          "Babylonjs": os.path.join(os.path.expanduser('~'), "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "src"),
          "Babylonjs - Unit Tests": os.path.join(os.path.expanduser('~'),
                                  "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "tests", "unit", "babylon"),
          "MaterialsLibrary": os.path.join(os.path.expanduser('~'), "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "materialsLibrary", "src"),
          "ProceduralTexturesLibrary": os.path.join(os.path.expanduser('~'),
                                  "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "proceduralTexturesLibrary", "src"),
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
