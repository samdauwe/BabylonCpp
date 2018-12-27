#!/usr/bin/env python

class Tools(object):

    def __init__(self):
        pass

    def createDir(self, path):
        '''
        Creates the specified directory if not exists.
        '''
        import errno, os
        # create dir
        try:
            if not os.path.isdir(path):
                os.makedirs(path)
        except OSError as exception:
            if exception.errno != errno.EEXIST:
                raise

    def isSubirectory(self, directory, subDirectory):
        '''
        Checks whether a directory is a sub directory of another directory.
        '''
        import os
        # Make both paths absolute
        directory = os.path.abspath(directory)
        subDirectory = os.path.abspath(subDirectory)
        # return true, if the common prefix of both is equal to directory
        return os.path.commonprefix([directory, subDirectory]) == directory

    def getCPUCount(self):
        '''
        Returns the number of CPUs in the system.
        '''
        import multiprocessing
        return multiprocessing.cpu_count()

    def removeDir(self, path):
        '''
        Deletes the folders in the given path recursively.
        '''
        import errno, os, shutil
        if os.path.isdir(path):
            try:
                shutil.rmtree(path)
            except OSError as error:
                if error.errno != errno.ENOENT:
                    raise

    def runCommand(self, directory, command, extraEnv=None):
        '''
        Executes the command in the specified directory.
        '''
        import os, subprocess
        try:
            basestring
        except NameError:
            basestring = str
        shell = isinstance(command, basestring)
        env = os.environ.copy()
        if extraEnv:
            env.update(extraEnv)
        print("Executing command '%s' in directory '%s'" % (command, directory))
        subprocess.check_call(command, shell=shell, env=env, cwd=directory)

class Build(object):

    def __init__(self):
        '''
        Constructor
        '''
        import argparse, sys
        progName = argparse.ArgumentParser().prog
        parser = argparse.ArgumentParser(
            description='%s' % progName,
            usage='''%s <command> [<args>]

The supported build commands are:
    all [<options>]             builds all the targets
        supported build options:
            [--mode=<mode>]     the build mode, should we either 'release' or 'debug' mode
    clean_all                   cleans all the build directories
    configure [<options>]       configures the cmake project
        supported configuration options:
            [--mode=<mode>]     the build mode, should we either 'release' or 'debug' mode
    devenv                      starts the IDE (Visual Studio on Windows) and loads the project
        supported build options:
            [--mode=<mode>]     the build mode, should we either 'release' or 'debug' mode
    update                      fetches sources from master and integrates then into the local branch
    update submodules           fetches latest version of all git submodules
''' % progName)
        parser.add_argument('command', help='Subcommand to run')
        args = parser.parse_args(sys.argv[1:2])
        if not hasattr(self, args.command):
            print('Unrecognized command')
            parser.print_help()
            exit(1)
        # helper vars
        self._tools = Tools()
        self._buildDir = None
        self._buildMode = "release"
        # use dispatch pattern to invoke method with same name
        getattr(self, args.command)()

    def all(self):
        '''
        Builds all the targets.
        '''
        import argparse, sys
        parser = argparse.ArgumentParser(description='build all')
        parser.add_argument('--mode', type=str, dest='mode',
                            choices = ['release', 'debug'], action='store',
                            default='release', help="the build mode, ' \
                            'should we either 'release' or 'debug' mode.")
        args = parser.parse_args(sys.argv[2:])
        self._cmakeConfigure(args)
        self._doBuild(args)

    def clean_all(self):
        '''
        Removes all the build directories.
        '''
        # - Remove the release build directory
        self._tools.removeDir(self._getBuildDirectory(releaseBuild=True))
        # - Remove the debug build directory
        self._tools.removeDir(self._getBuildDirectory(releaseBuild=False))

    def configure(self):
        '''
        Parses the configure command and runs the cmake configure command.
        '''
        import argparse, sys
        parser = argparse.ArgumentParser(description='configure')
        parser.add_argument('--mode', type=str, dest='mode',
                            choices = ['release', 'debug'], action='store',
                            default='release', help="the build mode, ' \
                            'should we either 'release' or 'debug' mode.")
        args = parser.parse_args(sys.argv[2:])
        self._cmakeConfigure(args)

    def devenv(self):
        '''
        Parses the devenv command and starts the IDE.
        '''
        import argparse, sys
        parser = argparse.ArgumentParser(description='devenv')
        parser.add_argument('--mode', type=str, dest='mode',
                            choices = ['release', 'debug'], action='store',
                            default='release', help="the build mode, ' \
                            'should we either 'release' or 'debug' mode.")
        args = parser.parse_args(sys.argv[2:])
        self._devenv(args)

    def update(self):
        '''
        Fetches sources from master and integrates then into the local branch.
        '''
        import os, sys
        updateArgs = sys.argv[2:]
        if len(updateArgs) == 0:
            # Pull latest version from master
            gitPullCmd = ['git', 'pull', 'origin', 'master']
            self._tools.runCommand(os.getcwd(), gitPullCmd)
        else:
            updateMode = updateArgs[0]
            # Pull latest version of all git submodules
            if updateMode == 'submodules':
                gitPullCmd = ['git', 'submodule', 'update', '--recursive',
                              '--remote']
                self._tools.runCommand(os.getcwd(), gitPullCmd)

    def _getBuildDirectory(self, releaseBuild = True):
        """
        Returns the name of the build directory.
        """
        return ["debug_build", "release_build"][releaseBuild]

    def _cmakeConfigure(self, args):
        '''
        Runs the cmake configure command.
        '''
        # determine the build directory name
        isReleaseBuild = (args.mode == 'release')
        self._buildDir = self._getBuildDirectory(isReleaseBuild)
        self._buildMode = args.mode
        # create the build directory
        self._tools.createDir(self._buildDir)
        # get the cmake options
        cmakeOptions = self._getCMakeOptions(
                        releaseBuild = isReleaseBuild
                       )
        # Configures the cmake project
        cmakeCmd = ["cmake", "../"] + cmakeOptions
        self._tools.runCommand(self._buildDir, cmakeCmd)

    def _devenv(self, args):
        '''
        Starts the IDE (Visual Studio on Windows) and loads the project
        '''
        # determine the build directory name
        isReleaseBuild = (args.mode == 'release')
        self._buildDir = self._getBuildDirectory(isReleaseBuild)
        self._buildMode = args.mode
        # start the IDE
        devenvCommand = self.getDevenvCommand()
        self._tools.runCommand(self._buildDir, devenvCommand)

    def _getCMakeOptions(self, releaseBuild = True):
        """
        Returns the cmake options.
        """
        # build mode
        cmakeOptions = ["-DCMAKE_BUILD_TYPE=%s" % \
                       ["Debug", "Release"][releaseBuild]]
        # cmake generator
        cmakeOptions += ["-G", self.getCMakeGenerator()]
        return cmakeOptions

    def _doBuild(self, args):
        """
        Returns the build.
        """
        # determine the build directory name
        isReleaseBuild = (args.mode == 'release')
        self._buildDir = self._getBuildDirectory(isReleaseBuild)
        self._buildMode = args.mode
        # run the build
        buildCommand = self.getBuildCommand()
        self._tools.runCommand(self._buildDir, buildCommand)

class LinuxBuild(Build):

    def __init__(self):
        '''
        Constructor.
        '''
        Build.__init__(self)

    def getCMakeGenerator(self):
        '''
        Returns the CMake generator.
        '''
        return 'Unix Makefiles'

    def getDevenvCommand(self):
        '''
        Returns command to start the IDE.
        '''
        return ''

    def getPlatformKey(self):
        '''
        Returns the platform key.
        '''
        import platform, sys
        bitness = '64' if platform.architecture()[0] == '64bit' else '32'
        if sys.platform == 'linux' or sys.platform == 'linux2':
            return 'linux_x86_%s' % bitness
        return 'unknown'

    def getBuildCommand(self, verbose=False):
        '''
        Returns the build command.
        '''
        # Create the make command
        buildCommand = ["make"]
        if verbose:
            # - Enable verbose mode
            buildCommand += ["--debug=v"]
        # - Specifies the number of jobs to run simultaneously
        buildCommand += ["--jobs=%d" % self._tools.getCPUCount()]
        return buildCommand

class WindowsBuild(Build):

    def __init__(self, msvcVer='vs2017Community'):
        '''
        Constructor.
        '''
        # Microsoft Visual Studio defintions
        self._msvcDefs = {
            # Microsoft Visual Studio 2017 Community Version
            'vs2017Community': {
                'productName': 'Visual Studio 2017',
                'versionNumber': '15.0',
                'cmakeGeneratorPlatform': 'Visual Studio 15 2017 Win64',
                'path': 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\Auxiliary\\Build\\vcvars64.bat'
            }
        }
        self._msvcDef = self._msvcDefs[msvcVer]
        # init superclass
        Build.__init__(self)

    def getCMakeGenerator(self):
        '''
        Returns the CMake generator for Microsoft Visual Studio.
        '''
        return self._msvcDef['cmakeGeneratorPlatform']

    def getDevenvCommand(self):
        '''
        Returns command to start Visual Studio.
        '''
        return 'start BabylonCpp.sln'

    def getPlatformKey(self):
        '''
        Returns the platform key.
        '''
        import platform, sys
        bitness = '64' if platform.architecture()[0] == '64bit' else '32'
        if sys.platform == 'win32':
            return 'win32_%s' % bitness
        return 'unknown'

    def getBuildCommand(self, verbose=False):
        '''
        Returns the build command.
        '''
        import os
        # Create batch file for running the build
        batchFileName = 'runMSVCBuild.bat'
        batchFile = os.path.join(self._buildDir, batchFileName)
        projectFile = 'BabylonCpp.sln'
        # Build swicthes
        switches = []
        # - Build mode
        buildMode = {'release': 'Release', 'debug': 'Debug'}[self._buildMode]
        switches += ['/p:Configuration=%s' % buildMode]
        # - Maximum number of concurrent processes to use when building
        switches += ['/m']
        # - Amount of information to display in the build log
        switches += ['/v:minimal']
        # Create build script
        with open(batchFile, "w") as f:
            # Run the Visual Studio developer command prompt
            f.write('call "%s"\n' % self._msvcDef['path'])
            f.write('msbuild %s %s\n' % (projectFile, ' '.join(switches)))
        buildCommand = batchFileName
        return buildCommand

def main():
    # Platform dependent build
    import sys
    if sys.platform == 'linux' or sys.platform == 'linux2':
        # linux OS
        LinuxBuild()
    elif sys.platform == 'win32':
        # Windows OS
        WindowsBuild()

if __name__ == '__main__':
    main()
