from bab_types import *
from file_utils import *
from snake_case_vs_CamelCase import *

def ClassName_from_filename(file: FileFullPath):
    basename = file.split("/")[-1]
    basename_no_extension = basename.replace(".cpp", "").replace(".h", "")
    class_name = ToCamelCase(basename_no_extension)
    return class_name



def remove_include(h_file_to_remove: FileFullPath, src_file_to_process: FileFullPath):
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


def is_include_line(line: str) -> bool:
    return line.startswith("#include")


def _find_all_struct_classes_in_header(h_file: FileFullPath) -> List[ClassName]:
    if "iscene_producer" in h_file:
        print("ah")
    lines = read_file_lines_no_eol(h_file)
    def is_decl(i, line):
        is_decl = False
        if line.startswith("class ") or line.startswith("struct "):
            for i_next in range(i, len(lines)):
                next_line = lines[i_next]
                idx1 = next_line.find(";")
                idx2 = next_line.find("{")                
                if idx1 >= 0 and idx2 >= 0:
                    is_decl = (idx2 < idx1)
                if idx1 < 0 and idx2 < 0:
                    continue
                if idx2 >=0 and idx1 <0:
                    is_decl = True
                    break
                if idx1 >= 0 and idx2 < 0:
                    is_decl = False
                    break
        return is_decl
    def extract_decl_name(decl_line):
        s = decl_line.strip().replace("{", "").strip()
        s = s.replace("BABYLON_SHARED_EXPORT ", "")
        s = s.replace("class ", "")
        s = s.replace("struct ", "")
        if s.find(":") > 0:
            s = s[:s.find(":")].strip()
        if "<" in s: 
            s = s[:s.find("<")].strip()
            print("Arg")
        return s

    all_classes = []
    for i, line in enumerate(lines):
        if is_decl(i, line):
            decl_name = extract_decl_name(line)
            all_classes.append(decl_name)
    return all_classes


def find_all_struct_classes_in_headers(all_h_files: List[FileFullPath])  \
    -> Dict[FileIncludePath, List[ClassName]]:

    classes_per_header = {}
    for i, h_file in enumerate(all_h_files):
        print("Processing {} {}/{}".format(h_file, i, len(all_h_files)))
        classes = _find_all_struct_classes_in_header(h_file)
        classes_per_header[make_babylon_include_path(h_file)] = classes
    return classes_per_header
