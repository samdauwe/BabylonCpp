
import os

SAMPLES_DIR = os.path.dirname(os.path.realpath(__file__ +  "/.."))
SAMPLES_DIR = SAMPLES_DIR.replace("\\", "/")
H_DIR = SAMPLES_DIR + "/include"
CPP_DIR = SAMPLES_DIR + "/src"

def ToCamelCase(snake_case: str) -> str:
    items  = snake_case.split("_")
    def UpperFirstLetter(word):
        if len(word) == 0:
            return ""
        return word[:1].upper() + word[1:]    
    itemsCamel = map(UpperFirstLetter, items)
    r = "".join(itemsCamel)
    return r

def files_with_extension(folder, extension) -> [str]:
    r = []
    ext_len = len(extension)
    for root, dirs, files in os.walk(folder):
        for file in files:
            if file[-ext_len:] == extension:
                full_file = root + "/" + file
                full_file = full_file.replace("\\", "/")
                r.append(full_file)
    return r

H_FILES = files_with_extension(H_DIR, ".h")
CPP_FILES = files_with_extension(CPP_DIR, ".cpp")

def read_file_lines_no_eol(filename):
    with open(filename, "r") as f:
        lines = f.readlines()
    lines_no_eol = []
    for line in lines:
        if line[-1:] == "\n":
            lines_no_eol.append(line[:-1])
        else:
            lines_no_eol.append(line)
    return lines_no_eol

def write_file_lines_no_eol(filename, lines):
    content = "\n".join(lines)
    with open(filename, "w") as f:
        f.write(content)

def remove_include(h_file_to_remove, src_file_to_process):
    lines = read_file_lines_no_eol(src_file_to_process)
    def keep_line(line):
        header_file_quoted = "<" + h_file_to_remove + ">"
        match = ("#include" in line) and (header_file_quoted in line)
        if match:
            print("match!")
        return not match
    lines_filtered = list(filter(keep_line, lines))
    if lines_filtered != lines:
        write_file_lines_no_eol(src_file_to_process, lines_filtered)
        print("{} file was impacted".format(src_file_to_process))

def compute_header_include_path(header_full_path):
    r = header_full_path.replace(H_DIR + "/", "")
    return r

def remove_all_includes(h_file_to_remove):
    for f in CPP_FILES:
        remove_include(h_file_to_remove, f)
    for f in H_FILES:
        remove_include(h_file_to_remove, f)

def compute_header_full_path(header_basename):
    for f in H_FILES:
        if header_basename in f:
            return f
    return None

def compute_cpp_full_path(cpp_basename):
    for f in CPP_FILES:
        if cpp_basename in f:
            return f
    return None


def remove_header_guard(header_full_path):
    lines = read_file_lines_no_eol(header_full_path)
    if len(lines) < 4:
        return
    has_header_guard = False
    if lines[0][:7] == "#ifndef":
        header_guard_name = lines[0][8:]
        has_header_guard = True
    if not has_header_guard:
        return
    lines_filtered = []
    for i in range(len(lines)):
        line = lines[i]
        keep_line = True
        if header_guard_name in line:
            keep_line = False
        if ("#endif" in line): 
            if (i < len(lines) - 1):
                next_line = lines[i + 1]
                if header_guard_name in next_line:
                    keep_line = False
        if keep_line:
            lines_filtered.append(line)
    write_file_lines_no_eol(header_full_path, lines_filtered)


def repair_see_links(header_full_path):
    lines = read_file_lines_no_eol(header_full_path)
    content_rewritten = ""
    i = 0
    while i < len(lines):
        line = lines[i]
        if (line == " * @see") and  (i < len(lines) - 1):
            next_line = lines[i +  1]
            content_rewritten = content_rewritten + " * @see " + next_line.replace(" * ", "") + "\n"
            i = i + 1
        else:
            content_rewritten = content_rewritten + line + "\n"
        i = i + 1
    # print(content_rewritten)
    with open(header_full_path, "w") as f:
        f.write(content_rewritten)


def is_include_line(line: str):
    return line.startswith("#include")
def is_not_include_line(line: str):
    return not is_include_line(line)

def move_header_code_to_cpp(header_full_path, cpp_full_path):
    lines_cpp = read_file_lines_no_eol(cpp_full_path)
    lines_h = read_file_lines_no_eol(header_full_path)
    
    lines_include = list(filter(is_include_line, lines_cpp)) + list(filter(is_include_line, lines_h))

    lines_cpp_new = (
            lines_include
         +  list(filter(is_not_include_line, lines_h) )
         +  list(filter(is_not_include_line, lines_cpp) )
    )
    write_file_lines_no_eol(cpp_full_path, lines_cpp_new)
    os.remove(header_full_path)

def add_include_samples_index_to_cpp(cpp_full_path):
    lines_cpp = read_file_lines_no_eol(cpp_full_path)
    samples_index_h = "#include <babylon/samples/samples_index.h>"
    if samples_index_h not in lines_cpp:
        lines_cpp = [ samples_index_h ] + lines_cpp
    write_file_lines_no_eol(cpp_full_path, lines_cpp)

def SampleClassName_from_cpp_full_path(cpp_full_path):
    basename = cpp_full_path.split("/")[-1]
    snake_case_name = basename.replace(".cpp", "")
    ClassName = ToCamelCase(snake_case_name)
    return ClassName

def register_example_scene(cpp_full_path, sample_category):
    lines_cpp = read_file_lines_no_eol(cpp_full_path)
    is_registered = False
    for line in lines_cpp:
        if "BABYLON_REGISTER_SAMPLE" in line:
            is_registered = True
    if is_registered:
        return

    if len(lines_cpp) < 4:
        return
    
    sampleCamelCaseName = SampleClassName_from_cpp_full_path(cpp_full_path)
    register_line = "BABYLON_REGISTER_SAMPLE(\"" + sample_category + "\", " + sampleCamelCaseName + ")"
    lines_cpp = lines_cpp[:-2] + [register_line] + lines_cpp[-2:]
    write_file_lines_no_eol(cpp_full_path, lines_cpp)


def make_headerless_sample(filename_no_extension, sample_category):
    h_full_path = compute_header_full_path(filename_no_extension + ".")
    cpp_full_path = compute_cpp_full_path(filename_no_extension + ".")
    # print(h_full_path)
    # print(cpp_full_path)
    if h_full_path is not None:
        h_include_path = compute_header_include_path(h_full_path)
    # print(h_include_path)

    if h_full_path is not None:
        remove_all_includes(h_include_path)
        remove_header_guard(h_full_path)
        repair_see_links(h_full_path)
        move_header_code_to_cpp(h_full_path, cpp_full_path)

    add_include_samples_index_to_cpp(cpp_full_path)
    register_example_scene(cpp_full_path, sample_category)


filename_no_extension = "animated_cube_scene"
sample_category = "Loaders - glTF format"
make_headerless_sample(filename_no_extension, sample_category)
