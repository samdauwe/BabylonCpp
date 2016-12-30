import os

def rreplace(s, old, new, occurrence = 1):
    """
    Replaces the last occurence(s) of an expression in a string.
    """
    return new.join(s.rsplit(old, occurrence))

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

def shaderFilenameToVariableName(filename, isInclude = False):
    """
    Determines the variable name from the filename.
    """
    split = filename.split(".")[:-1] 
    if not isInclude:
        split += ["shader"]
    for i in range(1, len(split)):
        split[i] = "%s%s" % (split[i][0].upper(), split[i][1:])
    varName = ''.join(split)
    if varName.endswith("FragmentShader"):
        varName = varName.replace("FragmentShader", "PixelShader")
    return varName

def processShaderFile(sourcedir, shaderPath, outputDir, isInclude = False):
    """
    Puts the shader file content into a header file.
    """
    import codecs
    # read shader file contents
    shaderFilename = os.path.basename(shaderPath)
    lines = [line.rstrip('\n').rstrip('\r').decode("utf-8-sig") \
                                                for line in open(shaderPath)]
    # generate header file content
    outputFileName = "%s.h" % shaderFilename.replace(".", "_")
    defineName = "BABYLON_SHADERS_%s" % outputFileName.replace(".", "_").upper()
    if isInclude:
        defineName = defineName.replace("BABYLON_SHADERS_",
                                        "BABYLON_SHADERS_SHADERS_INCLUDE_")
    varName = shaderFilenameToVariableName(shaderFilename, isInclude)
    eol = '\n'
    output = "#ifndef %s%s" % (defineName, eol)
    output += "#define %s%s%s" % (defineName, eol, eol)
    output += "namespace BABYLON {%s%s" % (eol, eol)
    # process first line
    output += "extern const char* %s;%s%s" % (varName, eol, eol)
    output += "const char* %s%s  = " % (varName, eol)
    if lines[0] == "precision highp float;":
        output += "\"%s\\n\"%s" % ("#ifdef GL_ES", eol)
        output += "    \"%s\\n\"%s" % (lines[0], eol)
        output += "    \"%s\\n\"%s" % ("#endif", eol)
    else:
        output += "\"%s\\n\"%s" % (lines[0], eol)
    # process remainder of the shader file
    for i in range(1, len(lines)-1):
        lines[i] = lines[i].replace('\"', "\\\"")
        lines[i] = lines[i].replace('\t', " " * 2)
        while lines[i].startswith(" " * 4):
            lines[i] = lines[i].replace(" " * 4, " " * 2)
        if lines[i] == "precision highp float;":
            output += "    \"%s\\n\"%s" % ("#ifdef GL_ES", eol)
            output += "    \"%s\\n\"%s" % (lines[i], eol)
            output += "    \"%s\\n\"%s" % ("#endif", eol)
        else:
            output += "    \"%s\\n\"%s" % (lines[i], eol)
    output += "    \"%s\\n\";%s%s" % (lines[-1], eol, eol)
    output += "} // end of namespace BABYLON%s%s" % (eol, eol)
    output += "#endif // end of %s%s" % (defineName, eol)
    # write header file content to file
    outputFileLocation = os.path.join(outputDir, outputFileName)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    return outputFileLocation

def generateShadersStore(shaderFiles, outputDir,
                         outputFileName = "effect_shaders_store.cpp"):
    """
    Generates the "effect_shaders_store.cpp" file containing the shaders map.
    """
    import codecs
    ### generate header file ###
    eol = '\n'
    headerFilename = outputFileName.replace(".cpp", ".h")
    output = "#ifndef BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H%s" % eol
    output += "#define BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H%s%s" % (eol,eol)
    output += "#include <babylon/babylon_global.h>%s%s" % (eol, eol)
    output += "namespace BABYLON {%s%s" % (eol, eol)
    output += "struct BABYLON_SHARED_EXPORT EffectShadersStore {%s" % eol
    output += "  static std::unordered_map<std::string, const char*> "
    output += "Shaders;%s" % eol
    output += "}; // end of struct EffectShadersStore%s%s" % (eol, eol)
    output += "} // end of namespace BABYLON%s%s#endif " % (eol, eol)
    output += "// end of BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H%s" % eol
    # write output to file
    hdir = rreplace(outputDir, "src", os.path.join("include", "babylon"), 1)
    outputFileLocation = os.path.join(hdir, headerFilename)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    ### generate source file containg the shader map ###
    output = "#include <babylon/materials/%s>%s%s" % \
                                (outputFileName.replace(".cpp", ".h"), eol, eol)
    shaderNames = []
    for shaderFile in shaderFiles:
        shaderFilename = os.path.basename(shaderFile)
        shaderNames.append(shaderFilenameToVariableName(shaderFilename))
        output += "#include <babylon/shaders/%s>%s" % ("%s.h" % \
                                        shaderFilename.replace(".", "_"), eol)
    output += "%snamespace BABYLON {%s%s" % (eol, eol, eol)
    output += "std::unordered_map<std::string, const char*> "
    output += "EffectShadersStore::Shaders = {%s" % eol
    for shaderName in shaderNames:
        output += "  {\"%s\", %s},%s" % (shaderName, shaderName, eol)
    output = "%s%s};%s%s" % (output[:-2], eol, eol, eol)
    output += "} // end of namespace BABYLON%s" % eol
    # write output to file
    outputFileLocation = os.path.join(outputDir, outputFileName)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    return outputFileLocation

def generateIncludesShadersStore(shaderFiles, outputDir,
                        outputFileName = "effect_includes_shaders_store.cpp"):
    """
    Generates the "effect_includes_shaders_store.cpp" file containing the
    shaders includes map.
    """
    import codecs
    ### generate header file ###
    eol = '\n'
    headerFilename = outputFileName.replace(".cpp", ".h")
    output = "#ifndef BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H%s" % eol
    output += "#define BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H%s%s"  \
                                                                    % (eol,eol)
    output += "#include <babylon/babylon_global.h>%s%s" % (eol, eol)
    output += "namespace BABYLON {%s%s" % (eol, eol)
    output += "struct BABYLON_SHARED_EXPORT EffectIncludesShadersStore {%s"% eol
    output += "  static std::unordered_map<std::string, const char*> "
    output += "Shaders;%s" % eol
    output += "}; // end of struct EffectIncludesShadersStore%s%s" % (eol, eol)
    output += "} // end of namespace BABYLON%s%s#endif " % (eol, eol)
    output += "// end of BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H%s"  \
                                                                        % eol
    # write output to file
    hdir = outputDir.replace("source", os.path.join("include", "babylon"))
    outputFileLocation = os.path.join(hdir, headerFilename)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    ### generate source file containg the shader map ###
    output = "#include <babylon/materials/%s>%s%s" % (headerFilename, eol, eol)
    shaderNames = []
    for shaderFile in shaderFiles:
        shaderFilename = os.path.basename(shaderFile)
        shaderNames.append(shaderFilenameToVariableName(shaderFilename, True))
        output += "#include <babylon/shaders/shadersinclude/%s>%s" % ("%s.h" % \
                                        shaderFilename.replace(".", "_"), eol)
    output += "%snamespace BABYLON {%s%s" % (eol, eol, eol)
    output += "std::unordered_map<std::string, const char*> "
    output += "EffectIncludesShadersStore::Shaders  %s = {" % eol
    for shaderName in shaderNames:
        output += "{\"%s\", %s},%s   " % (shaderName, shaderName, eol)
    output = "%s%s};%s%s" % (output[:-5], eol, eol, eol)
    output += "} // end of namespace BABYLON%s" % eol
    # write output to file
    outputFileLocation = os.path.join(outputDir, outputFileName)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    return outputFileLocation

def sortDictionaryByKey(directory):
    """
    Sorts a dictonary by key in ascending order.
    """
    import collections
    return collections.OrderedDict(sorted(directory.items()))

def getFilesnamesInDir(directory, shaderExtension = ".fx"):
    """
    Returns all the files in the specified directory.
    """
    files = {}
    for f in os.listdir(directory):
        fileLocation =  os.path.join(directory, f)
        if os.path.isfile(fileLocation) and f.endswith(shaderExtension):
            files[f] = fileLocation
    return files

def getDirectoriesInDir(directory):
    """
    Returns all the directories in the specified directory.
    """
    directories = {}
    for d in os.listdir(directory):
        path =  os.path.join(directory, d)
        if os.path.isdir(path):
            directories[d] = path
    return directories

def generateShadersStores(shadersInputDir, shadersOutputDir, shadersStoreDir,
                          processProceduralTextures = False,
                          processMaterialsLibrary = False):
    """
    Generates the shaders stores files from the Babylon.js shaders input folder.
    """
    #### process the shader files ####
    import time
    # - standard shaders
    shaderFiles = getFilesnamesInDir(shadersInputDir)
    dlc = len(shaderFiles)
    # - procedural textures
    ptlc = 0
    if processProceduralTextures:
        proceduralTexturesInputDir = os.path.join(shadersInputDir, "../../",
                                                    "proceduralTexturesLibrary",
                                                    "proceduralTextures")
        if os.path.isdir(proceduralTexturesInputDir):
            procTexturesDirs = getDirectoriesInDir(proceduralTexturesInputDir)
            for shaderName, path in procTexturesDirs.items():
                proceduralTextures = getFilesnamesInDir(path)
                shaderFiles.update(proceduralTextures)
                ptlc += len(proceduralTextures)
    # - materials library
    mlc = 0
    if processMaterialsLibrary:
        materialsLibraryInputDir = os.path.join(shadersInputDir, "../../",
                                                    "materialsLibrary",
                                                    "materials")
        if os.path.isdir(materialsLibraryInputDir):
            materialsLibraryDirs = getDirectoriesInDir(materialsLibraryInputDir)
            for shaderName, path in materialsLibraryDirs.items():
                materials = getFilesnamesInDir(path)
                shaderFiles.update(materials)
                mlc += len(materials)
    # - sort shader files by filename
    shaderFiles = sortDictionaryByKey(shaderFiles)
    # - process shader files
    tic = time.time()
    for filename, shaderPath in shaderFiles.items():
        outputFileLocation = processShaderFile(shadersInputDir, shaderPath,
                                               shadersOutputDir)
        print "Written file: %s" % outputFileLocation
    toc = time.time()
    indent = ' ' * 11
    print "Processed %d shader files in %ss" % (len(shaderFiles), (toc - tic))
    print "%s|-%d shaders found in default library" % (indent, dlc)
    print "%s|-%d shaders found in procedural textures library" % (indent, ptlc)
    print "%s|-%d shaders found in materials library" % (indent, mlc)
    # generate the shader store
    generateShadersStore(shaderFiles, shadersStoreDir)

    ### process the includes shader files ###
    shadersInputDir = os.path.join(shadersInputDir, "ShadersInclude")
    shadersOutputDir = os.path.join(shadersOutputDir, "shadersinclude")
    createDir(shadersOutputDir)
    shaderFiles = getFilesnamesInDir(shadersInputDir)
    tic = time.time()
    for filename, shaderPath in shaderFiles.items():
        outputFileLocation = processShaderFile(shadersInputDir, shaderPath,
                                               shadersOutputDir, True)
        print "Written file: %s" % outputFileLocation
    toc = time.time()
    # generate the includes shader store
    generateIncludesShadersStore(shaderFiles, shadersStoreDir)
    print "Processed %d includes shader files in %ss" % (len(shaderFiles),
                                                         (toc - tic))

if __name__ == "__main__":
    from optparse import OptionParser

    parser = OptionParser(usage="usage: %prog [options] inputDir outputDir",
                          version="%prog 1.0")
    parser.add_option("-i", "--inputDir", action="store",
                      dest="shadersInputDir", type="string",
                      help="Babylon.js shaders path containing the " \
                           "source shaders")
    parser.add_option("-o", "--outputDir", action="store",
                      dest="shadersOutputDir", type="string",
                      help="Path where the C++ shader files should bes stored.")
    (options, args) = parser.parse_args()

    # set default values when input is missing
    if not options.shadersInputDir:
        options.shadersInputDir = os.path.join(os.getcwd(), "..", "..",
                                               "Babylon.js-2.5", "src",
                                               "Shaders")
        args += [options.shadersInputDir]
    if not options.shadersOutputDir:
        options.shadersOutputDir = os.path.join(os.getcwd(), "..", "src",
                                            "BabylonCpp", "include", "babylon",
                                            "shaders")
        args += [options.shadersOutputDir]

    # variables needed by the generator
    shadersInputDir = None
    shadersOutputDir = None
    shadersStoreDir = None

    # check arguments
    if len(args) != 2:
        if not options.shadersInputDir and not options.shadersOutputDir:
            parser.error("Incorrect number of arguments")

    # check if the shaders input folder exists
    if options.shadersInputDir and os.path.isdir(options.shadersInputDir):
        shadersInputDir = options.shadersInputDir
    else:
        parser.error("Incorrect input directory")

    # check if the shaders output folder exists
    if options.shadersOutputDir and os.path.isdir(options.shadersOutputDir):
        shadersOutputDir = options.shadersOutputDir
        tmp = shadersOutputDir.replace(os.path.join("include", "babylon",
                                                    "shaders"),
                                       os.path.join("src", "materials"))
        if os.path.isdir(tmp):
            shadersStoreDir = tmp
        else:
            parser.error("Incorrect output directory")
    else:
        parser.error("Incorrect output directory")

    # generate the shader store
    generateShadersStores(shadersInputDir, shadersOutputDir, shadersStoreDir)
