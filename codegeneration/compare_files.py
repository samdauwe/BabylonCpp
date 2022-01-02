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
    current, previous = "4.1.0", "4.1.0-beta.23"
    current, previous = "4.2.0", "4.2.0-beta.12"
    # Dictionary mapping from BabylonJs version to relative path
    BabylonJsVersions = {
        "4.1.0"         : "4.1.0_2020_02_27",
        "4.2.0"         : "4.2.0_2020_11_12",
    }
    # List containing the files to ignore
    whiteList = [
        # Common
        "index.ts",
        # Audio
        "analyser.ts", "audioEngine.ts", "audioSceneComponent.ts", "sound.ts",
        "soundTrack.ts", "weightedsound.ts",
        # Audio / Interfaces
        "IAudioEngine.ts",
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
        # DeviceInput
        "deviceInputSystem.ts", "deviceSourceManager.ts", "deviceTypes.ts",
        # Engines
        "nativeEngine.ts", "performanceConfigurator.ts",
        # Engines / Extensions
        "engine.videoTexture.ts", "engine.views.ts", "engine.webVR.ts",
        # Engines / Native
        "nativeShaderProcessor.ts",
        # Helpers
        "videoDome.ts",
        # LibDeclarations
        "browser.d.ts", "webgl.d.ts", "webgl2.d.ts", "webvr.d.ts", "webxr.d.ts",
        "webxr.nativeextensions.d.ts",
        # Loading
        "loadingScreen.ts",
        # Materials / Textures
        "basisTextureLoader.ts", "htmlElementTexture.ts", "videoTexture.ts",
        # Materials / Textures / Packer
        "frame.ts", "packer.ts",
        # Meshes / Compression
        "dracoCompression.ts",
        # Misc
        "andOrNotEvaluator.ts", "assetsManager.ts", "basis.ts", "baseError.ts",
        "canvasGenerator.ts", "copyTools.ts", "customAnimationFrameRequester.ts",
        "dataReader.ts", "DataStorage", "decorators.ts", "deepCopier.ts",
        "deferred.ts", "domManagement.ts", "fileRequest.ts", "filesInput.ts",
        "filesInputStore.ts", "instantiationTools.ts",
        "khronosTextureContainer2.ts", "logger.ts", "loadFileError.ts",
        "precisionDate.ts", "promise.ts", "retryStrategy.ts",
        "sceneRecorder.ts", "sceneSerializer.ts", "screenshotTools.ts",
        "smartArray.ts", "stringDictionary.ts", "tags.ts", "timer.ts",
        "trajectoryClassifier.ts", "typeStore.ts", "videoRecorder.ts",
        "virtualJoystick.ts", "webRequest.ts", "workerPool.ts",
        # Offline
        "database.ts", "IOfflineProvider.ts",
        # Physics / Plugins
        "ammoJSPlugin.ts", "cannonJSPlugin.ts", "oimoJSPlugin.ts",
        # XR
        "webXRInputSource.ts",
        # XR / features
        "WebXRAbstractFeature.ts", "WebXRControllerPhysics.ts",
        "WebXRControllerPointerSelection.ts", "WebXRControllerTeleportation.ts",
        "WebXRHitTest.ts",
        # XR / motionController
        "webXRAbstractMotionController.ts", "webXRControllerComponent.ts",
        "WebXRFeaturePointSystem.ts", "WebXRHandTracking.ts",
        "webXRHTCViveMotionController.ts", "webXRGenericMotionController.ts",
        "webXRMicrosoftMixedRealityController.ts",
        "webXRMotionControllerManager.ts",
        "webXROculusTouchMotionController.ts",
        "webXRProfiledMotionController.ts",
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
          "Inspector": os.path.join(os.path.expanduser('~'), "Projects",
                                  "Babylon.js-%s" % BabylonJsVersions[version],
                                  "inspector", "src"),
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
