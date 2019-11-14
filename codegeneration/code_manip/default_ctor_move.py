from bab_types import *
from file_utils import *
from snake_case_vs_CamelCase import *
from clang_warning_utils import *
from code_utils import *

def move_trivial_destructors_to_h(cpp_file: FileFullPath, all_h_files):
    ClassName = ClassName_from_filename(cpp_file)

    # process cpp file
    lines = read_file_lines_no_eol(cpp_file)
    has_trivial_destructor = False
    lines_new = []
    for line in lines:
        code_to_search = "::~{}(".format(ClassName).lower()
        if code_to_search in line.replace(" ", "").lower() and "=default" in line.replace(" ", "").lower():
            has_trivial_destructor = True
        else:
            lines_new.append(line)
    write_file_lines_no_eol(cpp_file, lines_new)

    # process h file
    h_file = h_file_from_cpp(cpp_file, all_h_files)
    if has_trivial_destructor:
        lines = read_file_lines_no_eol(h_file)
        lines_new = []
        for line in lines:
            code_to_search = "~{}(".format(ClassName).lower()
            if code_to_search in line.replace(" ", "").lower():
                line = line.replace(" // = default", "") [:-1] + " = default;"
            lines_new.append(line)
        write_file_lines_no_eol(h_file, lines_new)




def correct_one_trivial_destructor(cpp_file, cpp_file_line, h_file):
    ClassName = None

    # correct cpp file
    lines = read_file_lines_no_eol(cpp_file)
    line_with_warning = lines[cpp_file_line]
    index_start = line_with_warning.find("~")
    index_end = line_with_warning.find("(", index_start)
    if index_start < 0 or index_end < 0:        
        return
    ClassName = line_with_warning[index_start + 1:index_end]
    # code_to_search = "::~{}(".format(ClassName).lower()
    # need_process = False
    # if code_to_search in line_with_warning.replace(" ", "").lower():
    #     need_process = True
    # if not need_process:
    #     print("Argh")
    if True:
        exclusion_start = cpp_file_line
        exclusion_end = 0
        idx_line = exclusion_start
        found_closing = False
        while found_closing == False:
            line = lines[idx_line]
            if "}" in line:
                found_closing = True
            idx_line = idx_line + 1
        exclusion_end = idx_line + 1
        lines_new = []
        for idx, line in enumerate(lines):
            if idx >= exclusion_start and idx < exclusion_end:
                continue
            if idx == exclusion_end:
                lines_new.append("{}::~{}() = default;".format(ClassName, ClassName))
                lines_new.append("")
            lines_new.append(line)
        # print("\n".join(lines_new))
        write_file_lines_no_eol(cpp_file, lines_new)

    # correct h file
    if h_file is not None:
        lines = read_file_lines_no_eol(h_file)
        lines_new = []
        for line in lines:
            if "~" in line and ClassName + "()" in line:
                if "//" in line:
                    line = line[:line.index("//")]
                    line = line.rstrip()
                line = line.replace(" override ", "")
                line = line.replace("override ", "")
                line = line.replace(" override", "")
                line = line.replace("override", "")
                line = line + " // = default"

            lines_new.append(line)
        write_file_lines_no_eol(h_file, lines_new)



def correct_trivial_destructors(all_h_files):
    warning_lines = read_file_lines_no_eol("/home/pascal/dvp/BabylonCpp/warnings.txt")
    warning_destructor = filter(is_warning_trivial_destructor, warning_lines)
    locations = [warning_location(line) for line in warning_destructor]
    # print(locations)
    for i, location in enumerate(locations):
        cpp_file, cpp_file_line = location
        print("Processing {} {}/{}".format(i, cpp_file, len(locations)))
        h_file = h_file_from_cpp(cpp_file, all_h_files)
        correct_one_trivial_destructor(cpp_file, cpp_file_line, h_file)

