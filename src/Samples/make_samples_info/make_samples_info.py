import os
import os.path
import json
THIS_SCRIPT_DIR = (os.path.dirname(os.path.realpath(__file__)))
ROOT_DIR = (os.path.realpath(THIS_SCRIPT_DIR + "/../../.."))
SAMPLES_INCLUDE_DIR = ROOT_DIR + "/src/Samples/include/babylon/samples"
SAMPLES_SOURCE_DIR = ROOT_DIR + "/src/Samples/src/samples"

def all_files_with_extension(root_dir, extension):
    result = []
    for root, dirs, files in os.walk(root_dir):
        for filename in files:
            filename_with_path = root + "/" + filename
            if filename_with_path[-len(extension) :] == extension:
                result.append(filename_with_path)
    return result


HEADERS = all_files_with_extension(SAMPLES_INCLUDE_DIR, ".h")
SOURCES = all_files_with_extension(SAMPLES_SOURCE_DIR, ".cpp")


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
    result["header_file"] = header_file
    result["source_file"] = cpp_file
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
    infos = make_all_info()
    with open("samples_info.json", "w") as f:
        f.write(json.dumps(infos, indent=4))

if __name__ == "__main__":
    print("running make_samples_info.py")
    write_json_info_file()
