import os
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


def find_corresponding_source_file(sample_header_file):
    src = os.path.basename(sample_header_file)[:-2] + ".cpp"
    for s in SOURCES:
        if src in s:
            return s
    return "Not found"

def snake_case_to_lowersnakecase(s):
    items = s.split("_")
    result = "".join(items)
    result = result.lower()
    return result

def make_sample_name_from_filename(header_file):
    header_file = header_file.replace("\\", "/")
    header_file = header_file.split("/")[-1]
    header_file = header_file.replace(".h", "")
    sample_name = snake_case_to_lowersnakecase(header_file)
    return sample_name

def sample_info(header_file):
    result = dict()
    result["header_file"] = header_file
    result["source_file"] = find_corresponding_source_file(header_file)
    with open(header_file) as f:
        lines = f.readlines()

    # Find sample name (class or struct name)
    result["sample_name"] = make_sample_name_from_filename(header_file)

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
    for header_file in HEADERS:
        result.append(sample_info(header_file))
    return result

def write_json_info_file():
    infos = make_all_info()
    with open("samples_info.json", "w") as f:
        f.write(json.dumps(infos, indent=4))

if __name__ == "__main__":
    print("running make_samples_info.py")
    write_json_info_file()
