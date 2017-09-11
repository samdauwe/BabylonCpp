#!/usr/bin/env python

import os
import sys

def getPlatformKey():
    """
    Return the platform key (type of operating system and 32bit/64bit).
    """
    import platform
    osPrecision = '64' if platform.architecture()[0] == '64bit' else '32'  
    if sys.platform == "linux" or sys.platform == "linux2":
        # linux
        return 'Linux_x86_%s' % osPrecision
    elif sys.platform == "win32":
        # Windows...
        return 'Win32_%s' % osPrecision
    else:
        return 'Unknown'

def getCPUCount():
    """
    Return the number of CPUs in the system.
    """
    import multiprocessing
    return multiprocessing.cpu_count()

def getBuildDirectory(releaseBuild = True):
    """
    Returns the name of the build directory.
    """
    return ["debug_build", "release_build"][releaseBuild]

def isSubirectory(directory, subDirectory):
    """
    Checks whether a directory is a sub directory of another directory.
    """
    # Make both paths absolute
    directory = os.path.abspath(directory)
    subDirectory = os.path.abspath(subDirectory)
    # return true, if the common prefix of both is equal to directory
    return os.path.commonprefix([directory, subDirectory]) == directory

def removeDir(path):
    """
    Deleting the folders in path recursively.
    """
    import shutil
    if os.path.isdir(path) and isSubirectory(os.getcwd(), path):
        shutil.rmtree(path)

def createDir(path):
    """
    Creates the specified directory if not exists.
    """
    import errno
    # create dir
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

def createBuildDirectory(buildDirectory):
    """
    Creates the build directory.
    """
    if not os.path.isdir(buildDirectory):
        createDir(buildDirectory)
    return {"result": os.path.isdir(buildDirectory),
            "buildDirectory": buildDirectory}

def runCommand(command = []):
    """
    Executes a shell command and returns the return code.
    """
    from subprocess import call
    returnCode = 0
    errorMessage = ""
    try:
        call(command)
    except OSError as exception:
        returnCode = exception.errno
        errorMessage = str(exception)
    return {"returnCode": returnCode, "errorMessage": errorMessage}

def getCMakeOptions(releaseBuild = True):
    """
    Returns the cmake options.
    """
    cmakeOptions = ["-DCMAKE_BUILD_TYPE=%s" % \
                    ["Debug", "Release"][releaseBuild]]
    if getPlatformKey() == "Win32_64":
        cmakeOptions += ["-G", "Visual Studio 15 2017 Win64"]
    return cmakeOptions

def runCmakeConfigure(buildDirectory, cmakeOptions=[]):
    """
    Runs the cmake command and returns the return code.
    """
    # Create the build directory
    res = createBuildDirectory(buildDirectory)
    # Configures the cmake project
    cwd = os.getcwd()
    os.chdir(buildDirectory)
    cmakeCmd = ["cmake", "../"] + cmakeOptions
    # run command
    print "Running cmake command: %s" % " ".join(cmakeCmd)
    res = runCommand(cmakeCmd)
    os.chdir(cwd)
    return res

def startBuild(buildDirectory, cmakeOptions=[], verbose=False):
    """
    Starts the main cmake build.
    """
    # Create the build directory
    res = createBuildDirectory(buildDirectory)
    if res["result"]:
        # Run cmake
        res = runCmakeConfigure(buildDirectory, cmakeOptions)
        if res["returnCode"] == 0:
            # Create the build
            cwd = os.getcwd()
            os.chdir(buildDirectory)
            buildCommand = []
            if getPlatformKey() == "Linux_x86_64":
                # Create the make command
                buildCommand = ["make"]
                if verbose:
                    # - Enable verbose mode
                    buildCommand += ["--debug=v"]
                # - Specifies the number of jobs to run simultaneously
                buildCommand += ["--jobs=%d" % getCPUCount()]
            # run build command
            print "Running build command: %s" % " ".join(buildCommand)
            res = runCommand(buildCommand)
            os.chdir(cwd)
    return res

def getBuildTargets():
    """
    Returns a list with build targets.
    """
    return [("all", "builds all the targets"),
            ("configure", "configures the cmake project"),
            ("clean_all", "cleans all the build directories")]

def getTargetsHelpMessage():
    """
    Lists the supported targets for this build script.
    """
    msg = "The supported targets are the following:\n"
    for buildTarget in getBuildTargets():
        msg += "  %s : %s\n" % (buildTarget[0], buildTarget[1])
    return msg[:-1]

def main():
    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options]",
                          add_help_option=False,
                          version="%prog 1.0")
    parser.add_option("-h", "--help", dest="help", action="store_true",
                      help="show this help message and exit")
    parser.add_option("-d", "--debug", dest="debug", action="store_true",
                      help="enabled a debug build")
    parser.add_option("-t", "--target", type='choice', dest="target",
                      choices = [t[0] for t in getBuildTargets()],
                      action="store", default="all",
                      help="the build target to use.")
    (options, args) = parser.parse_args()

    # Prints the help message and exits
    if options.help:
        parser.print_help()
        print '\n%s' % getTargetsHelpMessage()
        sys.exit()

    # Check the type of build
    releaseBuild = True
    if options.debug:
        releaseBuild = False

    # Get the build directory
    buildDirectory = getBuildDirectory(releaseBuild)

    # Get the cmake options
    cmakeOptions = getCMakeOptions(releaseBuild)

    # Get the build target
    if options.target:
        buildTarget = options.target
        if buildTarget == "all":
            # Starts a full build
            startBuild(buildDirectory, cmakeOptions)
        elif buildTarget == "configure":
            # Configure target
            runCmakeConfigure(buildDirectory, cmakeOptions)
        elif  buildTarget == "clean_all":
            # Clean target
            # - Remove the release build directory
            removeDir(getBuildDirectory(releaseBuild=True))
            # - Remove the debug build directory
            removeDir(getBuildDirectory(releaseBuild=False))  

if __name__ == '__main__':
    main()