import os
import os.path
import json
THIS_SCRIPT_DIR = (os.path.dirname(os.path.realpath(__file__)))
ROOT_DIR = (os.path.realpath(THIS_SCRIPT_DIR + "/../../.."))
SAMPLES_DIR = ROOT_DIR + "/src/Samples"
SAMPLES_INCLUDE_DIR = SAMPLES_DIR + "/include/babylon/samples"
SAMPLES_SOURCE_DIR = SAMPLES_DIR + "/src/samples"
ASSETS_DIR = ROOT_DIR + "/assets"

def all_files_with_extension(root_dir, extension):
    result = []
    for root, dirs, files in os.walk(root_dir):
        for filename in files:
            filename_with_path = root + "/" + filename
            if filename_with_path[-len(extension) :] == extension:
                result.append(filename_with_path.replace("\\", "/"))
    return result

HEADERS = sorted(all_files_with_extension(SAMPLES_INCLUDE_DIR, ".h"))
SOURCES = sorted(all_files_with_extension(SAMPLES_SOURCE_DIR, ".cpp"))

def find_corresponding_header_file(cpp_file):
    src = os.path.basename(cpp_file)[:-4] + ".h"
    for s in HEADERS:
        if src in s:
            return s
    return ""

def snake_case_to_lowersnakecase(s):
    items = s.split("_")
    result = "".join(items)
    result = result.lower()
    return result

def make_sample_name_from_filename(source_file):
    source_file = source_file.replace("\\", "/")
    source_file = source_file.split("/")[-1]
    source_file = source_file.replace(".h", "")
    source_file = source_file.replace(".cpp", "")
    sample_name = snake_case_to_lowersnakecase(source_file)
    return sample_name

def sample_info(cpp_file):
    result = dict()
    header_file = find_corresponding_header_file(cpp_file)
    result["header_file"] = header_file.replace(SAMPLES_DIR + "/", "")
    result["source_file"] = cpp_file.replace(SAMPLES_DIR + "/", "")
    if len(header_file) == 0:
        with open(cpp_file) as f:
            lines = f.readlines()
    else:
        with open(header_file) as f:
            lines = f.readlines()

    # Find sample name (class or struct name)
    result["sample_name"] = make_sample_name_from_filename(cpp_file)

    # Find brief
    brief = ""
    brief_started = False
    brief_done = False
    for line in lines:
        if "@brief" in line:
            brief_started = True
        if ("@see" in line) or ("*/" in line):
            brief_done = True
        if brief_started and not brief_done:
            line = line.replace("\n", " ")
            line = line.replace(" * ", "")
            line = line.replace("@brief", "")
            brief = brief +  line
    result["brief"] = brief.strip()

    # Find links
    links = []
    for line in lines:
        if ("@see" in line):
            url = line.replace(" * @see ", "")
            url = url.replace("\n", "")
            links.append(url)
    result["links"] = links

    return result

def make_all_info():
    result = []
    for cpp_file in SOURCES:
        result.append(sample_info(cpp_file))
    return result

def write_json_info_file():
    print("running make_samples_info.py")
    infos = make_all_info()
    with open(ASSETS_DIR + "/samples_info.json", "w") as f:
        f.write(json.dumps(infos, indent=4))


def collect_category_samplename_pairs():
    r = []
    for source in SOURCES:
        sample_category = ""
        sample_class = ""
        with open(source) as f:
            lines = f.readlines()
        for line in lines:
            if "BABYLON_REGISTER_SAMPLE" in line:
                line = line.replace("BABYLON_REGISTER_SAMPLE", "").replace("(", "").replace(")", "").replace("\"", "")
                items = line.split(",")
                sample_category = items[0].strip()
                sample_class = items[1].strip()
        if len(sample_class) >0:
            r.append((sample_category, sample_class))
    return r


def make_samples_auto_declarations_code():
    intro = """#ifndef BABYLON_SAMPLES_AUTO_DECL_H    
#define BABYLON_SAMPLES_AUTO_DECL_H

// This file is generated automatically at each build. Do not modify it manually.

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_index.h>

#include <memory>

namespace BABYLON
{
namespace Samples
{
"""

    end = """
    """

    stub = "std::shared_ptr<IRenderableScene> make_##sampleClassName(ICanvas* canvas);\n"

    code  = intro
    category_samplename_pairs = collect_category_samplename_pairs()
    for category_samplename_pair in category_samplename_pairs:
        categoryName = category_samplename_pair[0]
        sampleClassName = category_samplename_pair[1]
        code_line = stub.replace("##categoryName", categoryName).replace("##sampleClassName", sampleClassName)
        code = code + code_line

    code = code + end
    return code



def make_auto_populate_samples_code():
    category_samplename_pairs = collect_category_samplename_pairs()
    intro = """
template<typename RegisterFunction>
inline void auto_populate_samples(RegisterFunction& registerFunction)
{
"""
    end = """
}

} // namespace Samples
} // namespace BABYLON

#endif // #ifndef BABYLON_SAMPLES_AUTO_DECL_H"
    """

    stub = "  registerFunction(\"categoryName\", \"sampleClassName\", make_sampleClassName);\n"

    code = intro
    for category_samplename_pair in category_samplename_pairs:
        categoryName = category_samplename_pair[0]
        sampleClassName = category_samplename_pair[1]
        code_line = stub.replace("categoryName", categoryName).replace("sampleClassName", sampleClassName)
        code  = code + code_line

    code  = code + end
    return code

def write_samples_auto_declarations_file():
    print("running write_samples_auto_declarations_file")
    code = make_samples_auto_declarations_code() + make_auto_populate_samples_code()
    with open(SAMPLES_INCLUDE_DIR + "/samples_auto_declarations.h", "w") as f:
        f.write(code)


if __name__ == "__main__":
    print("Running make_samples_info.py")
    write_json_info_file()
    write_samples_auto_declarations_file()
