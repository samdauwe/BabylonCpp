from bab_types import *
from file_utils import *

def remove_forward_class_decls(h_file: FileFullPath, classes_per_header):
    lines = read_file_lines_no_eol(h_file)
    h_file_include = make_babylon_include_path(h_file)
    def extract_forward_decl_class(forward_decl_line):
        ClassName = forward_decl_line.replace("class", "").replace("struct", "")
        ClassName = ClassName.replace(";", "").replace(" ", "")
        return ClassName
    def is_forward_decl(line):
        if (line.startswith("class ") or line.startswith("struct ")) and line.endswith(";"):
            class_name = extract_forward_decl_class(line)
            if class_name in classes_per_header[h_file_include]:
                return False
            return True
        return False
    def find_header_file(forward_decl_line):
        ClassName = extract_forward_decl_class(forward_decl_line)
        candidate_h_files = []
        for header, classes in classes_per_header.items():
            if ClassName in classes:
                candidate_h_files.append(header)
        if len(candidate_h_files) == 0:
            print("Argh")
            return None
        if len(candidate_h_files) > 1:
            print("Argh")
            candidate_h_files = sorted(candidate_h_files, key = len)
        header_file =candidate_h_files[0]
        return header_file


    def is_removable_forward_decl(line):
        return is_forward_decl(line) and find_header_file(line) is not None

    def extract_shared_ptr_class(ptr_decl_line):
        class_name = ptr_decl_line[ ptr_decl_line.index("<") + 1 : ptr_decl_line.index(">") ]
        class_name = class_name.strip()
        return class_name

    def is_Ptr_decl(line):
        if "Ptr " in line and line.startswith("using") and "shared_ptr<" in line:
            class_name = extract_shared_ptr_class(line)
            if class_name in classes_per_header[h_file_include]:
                return False
            return True
        return False

    def is_removable_Ptr_decl(line):
        if not is_Ptr_decl(line):
            return False
        class_name = extract_shared_ptr_class(line)
        for header, classes in classes_per_header.items():
            for class_name2 in classes:
                if class_name == class_name2:
                    return True
        return False


    def is_babylon_include(line):
        return "#include <babylon/" in line
    def extract_babylon_include(babylon_include_line):
        s = babylon_include_line.strip().replace("#include <", "")
        s = s.replace(">", "")
        return s

    def find_idx_line_last_babylon_include():
        idx_line_last_babylon_include = 0
        for i, line in  enumerate(lines):
            if is_babylon_include(line):
                idx_line_last_babylon_include = i
        return idx_line_last_babylon_include

    def process_new_babylon_includes():
        previous_includes_lines = list(filter(is_babylon_include, lines))
        previous_includes = list(map(extract_babylon_include, previous_includes_lines))
        # print(previous_includes)

        fwd_decls_lines = list(filter(is_removable_forward_decl, lines))
        additional_includes = list(map(find_header_file, fwd_decls_lines))
        # print(additional_includes)

        all_includes = previous_includes + additional_includes
        all_includes = sorted(all_includes)
        all_includes = unique(all_includes)
        # print(all_includes)
        return all_includes

    fwd_decls_lines = list(filter(is_removable_forward_decl, lines))
    if len(fwd_decls_lines) > 0:
        idx_line_last_babylon_include = find_idx_line_last_babylon_include()
        new_babylon_includes = process_new_babylon_includes()
        lines_new = []
        for i, line in enumerate(lines):
            if not is_removable_forward_decl(line) and not is_removable_Ptr_decl(line) and not is_babylon_include(line):
                lines_new.append(line)
            if i == idx_line_last_babylon_include:
                for include in new_babylon_includes:
                    include_line = "#include <{}>".format(include)
                    lines_new.append(include_line)

        # print("\n".join(lines_new))
        write_file_lines_no_eol(h_file, lines_new)
