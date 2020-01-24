
import os
import os.path
import typing

SAMPLES_DIR = os.path.dirname(os.path.realpath(__file__ +  "/.."))
SAMPLES_DIR = SAMPLES_DIR.replace("\\", "/")
H_DIR = SAMPLES_DIR + "/include"
CPP_DIR = SAMPLES_DIR + "/src"
H_FILES = []
CPP_FILES = []

def ToCamelCase(snake_case: str) -> str:
    items  = snake_case.split("_")
    def UpperFirstLetter(word):
        if len(word) == 0:
            return ""
        return word[:1].upper() + word[1:]    
    itemsCamel = map(UpperFirstLetter, items)
    r = "".join(itemsCamel)
    return r

def files_with_extension(folder, extension) -> typing.List[str]:
    r = []
    ext_len = len(extension)
    if not os.path.isdir(folder):
        print("ouch")
    for root, dirs, files in os.walk(folder):
        for file in files:
            if file[-ext_len:] == extension:
                full_file = root + "/" + file
                full_file = full_file.replace("\\", "/")
                r.append(full_file)
    return r

def fill_H_CPP_files():
    global H_FILES, CPP_FILES
    H_FILES = files_with_extension(H_DIR, ".h")
    CPP_FILES = files_with_extension(CPP_DIR, ".cpp")


def find_samples_in_category(category_folder):
    h_files = files_with_extension(H_DIR + "/babylon/samples/" + category_folder, ".h")
    samples = []
    for f in h_files:
        if f.endswith("_index.h"):
            continue
        sample = os.path.basename(f).replace(".h", "")
        samples.append(sample)
    return samples

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
    fill_H_CPP_files()

def add_include_samples_index_to_cpp(cpp_full_path):
    lines_cpp = read_file_lines_no_eol(cpp_full_path)
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

def move_method(
    cpp_full_path, 
    method_name, 
    add_spacing = True,
    add_override = False,
    ):

    lines = read_file_lines_no_eol(cpp_full_path)
    def find_decl_lines_number() -> int:
        def is_decl(line):
            return " " + method_name + "(" in line
        for i, line in enumerate(lines):
            if is_decl(line):
                return i
        return None

    def find_impl_lines_number() -> typing.List[int]:
        def is_impl_start(line):
            return "::" + method_name + "(" in line
        def count_open_close(line):
            count = 0
            for c in line:
                if c == "{":
                    count = count + 1
                if c == "}":
                    count = count - 1
            return count
        
        impl_lines = []
        has_started = False
        counter_open_close = 0
        has_found_opening = False
        for i, line in enumerate(lines):
            if is_impl_start(line):
                has_started = True
            if has_started:
                impl_lines.append(i)
                count  = count_open_close(line)
                if count > 0:
                    has_found_opening = True
                counter_open_close = counter_open_close + count
            if (counter_open_close == 0) and has_started and has_found_opening:
                break
        return impl_lines

    decl_line_number = find_decl_lines_number()
    if decl_line_number is None:
        return
    impl_lines_numbers = find_impl_lines_number()
    impl_lines_str = list(lines[i] for i in impl_lines_numbers)
    # print("\n".join(impl_lines_str))

    lines_rewritten = []
    ClassName = SampleClassName_from_cpp_full_path(cpp_full_path)
    for i in range(len(lines)):
        line = lines[i]
        if i != decl_line_number and (not i in impl_lines_numbers):
            lines_rewritten.append(line)
        if i == decl_line_number and len(impl_lines_str) > 0:
            # remove ClassName::
            impl_lines_str[0] = impl_lines_str[0].replace(ClassName + "::", "")
            if add_override:
                if ")" in impl_lines_str[0]:
                    impl_lines_str[0] = impl_lines_str[0] + " override"
                else:
                    impl_lines_str[1] = impl_lines_str[1] + " override"
            if add_spacing:
                impl_lines_str = list(map( lambda s : "  " + s, impl_lines_str ))
            lines_rewritten = lines_rewritten + impl_lines_str

    write_file_lines_no_eol(cpp_full_path, lines_rewritten)
    # print("\n".join(lines_rewritten))

def group_multiline_strings(cpp_full_path):
    def is_line_multiline_part(line):
        return line.endswith("\"")

    def unescape_line(line: str):
        r = line.strip()
        r = r.replace("\\n\"", "")
        r = r.replace("\\\"", "\"")
        r = r.replace("\\\\", "\\")
        if r.startswith("\""):
            r = r[1:]
        if r.endswith("\""):
            r = r[:-1]
        return r

    def format_multilines(line_elements):
        start = "R\"ShaderCode(\n"
        end   = ")ShaderCode\";"

        separator_if_too_long = (
              end[:-1] 
            + "\n// Shader code string too long for msvc, it is splitted / joined here\n"
            + start)
        nb_lines_max = 400

        output_lines = []

        first_line = line_elements[0]
        idx = first_line.index("\"")
        output_lines.append( first_line[:idx] + start + unescape_line(first_line[idx + 1:]) )

        
        for i, line in enumerate(line_elements):
            if i == 0:
                continue
            if i == len(line_elements) - 1:
                if line[-1:] == ";":
                    line = line[:-1]
            output_lines.append(unescape_line(line))
            if (i + 1) % nb_lines_max == 0:
                output_lines.append(separator_if_too_long)

        output_lines.append(end)
        # print("\n".join(output_lines))
        return output_lines


    lines = read_file_lines_no_eol(cpp_full_path)

    lines_rewritten = []

    lines_multi_is_started = False
    lines_multi = []
    for i, line in enumerate(lines):
        if lines_multi_is_started:
            lines_multi.append(line)
            if not is_line_multiline_part(line):
                lines_multi_is_started = False

                lines_rewritten = lines_rewritten + format_multilines(lines_multi)
        else:
            if is_line_multiline_part(line):
                lines_multi_is_started = True
                lines_multi = [ line ]
            else:
                lines_rewritten.append(line)

    # print( "\n".join(lines_rewritten) )
    write_file_lines_no_eol(cpp_full_path, lines_rewritten)


def remove_close_open_namespaces(cpp_full_path):
    what_to_search = """} // end of namespace Samples
} // end of namespace BABYLON


namespace BABYLON {
namespace Samples {
"""
    with open(cpp_full_path, "r") as f_i:
        content = f_i.read()
    content = content.replace(what_to_search, "")
    content = content.replace("\n\n\n\n\n", "\n\n")
    content = content.replace("\n\n\n\n", "\n\n")
    content = content.replace("\n\n\n", "\n\n")
    
    with open(cpp_full_path, "w") as f_o:
        f_o.write(content)


def make_headerless_sample(filename_no_extension, sample_category):
    h_full_path = compute_header_full_path("/" + filename_no_extension + ".")
    cpp_full_path = compute_cpp_full_path("/" + filename_no_extension + ".")
    if h_full_path is not None:
        h_include_path = compute_header_include_path(h_full_path)

    if h_full_path is not None:
        remove_all_includes(h_include_path)
        remove_header_guard(h_full_path)
        repair_see_links(h_full_path)
        move_header_code_to_cpp(h_full_path, cpp_full_path)

    add_include_samples_index_to_cpp(cpp_full_path)
    register_example_scene(cpp_full_path, sample_category)

    move_method(cpp_full_path, "getName", add_override=True, add_spacing=True)
    move_method(cpp_full_path, "initializeScene", add_override=True, add_spacing=True)

    ClassName = SampleClassName_from_cpp_full_path(cpp_full_path)
    move_method(cpp_full_path, ClassName, add_override=False, add_spacing=True)
    move_method(cpp_full_path, "~" + ClassName, add_override=False, add_spacing=True)

    group_multiline_strings(cpp_full_path)
    remove_close_open_namespaces(cpp_full_path)


def make_headerless_samples_in_category(category_name, category_folder):
    samples = find_samples_in_category(category_folder)
    for sample in samples:
        print("Processing " + sample)
        make_headerless_sample(sample, category_name)


def correct_shader_multilines():
    cpp_files = files_with_extension(CPP_DIR + "/samples/materials/shadermaterial", ".cpp")
    for cpp_full_path in cpp_files:
        print(cpp_full_path)
        group_multiline_strings(cpp_full_path)
    # print(cpp_files)

fill_H_CPP_files()

# category_name = "Shader Material"
# category_folder = "materials/shadermaterial"

# filename_no_extension = "shader_material_box_scene"
# make_headerless_sample(filename_no_extension, category_name)

# make_headerless_samples_in_category(category_name, category_folder)

correct_shader_multilines()
