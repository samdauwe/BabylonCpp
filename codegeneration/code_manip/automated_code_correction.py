import os
import os.path
from typing import *

from bab_types import *
from file_utils import *
from snake_case_vs_CamelCase import *
from code_utils import *
from default_ctor_move import *


def main():
    repo_dir = os.path.dirname(os.path.realpath(__file__ +  "/../.."))
    src_dir = repo_dir + "/src"
    all_h_files = files_with_extension(src_dir, ".h")
    all_cpp_files = files_with_extension(src_dir, ".cpp")
    all_src_files = all_cpp_files + all_h_files
    
    master_repo_src = "/home/pascal/dvp/BabylonCpp/src"

    for f_new in all_src_files:
        f_master = f_new.replace(src_dir, master_repo_src)
        has_bom_master = file_has_utf8_bom(f_master)
        has_crlf_master = file_has_windows_crlf(f_master)
        has_bom_new = file_has_utf8_bom(f_new)
        has_crlf_new = file_has_windows_crlf(f_new)
        if has_bom_master != has_bom_new or has_crlf_master !=has_crlf_new:
            print("Processing {} has_bom={}->{} crlf={}->{}"
                .format(f_new.replace(src_dir, ""), 
                has_bom_new, has_bom_master,
                has_crlf_new, has_crlf_master ))
            lines = read_file_lines_no_eol(f_new)
            write_file_lines_no_eol_formatted(f_new, lines, has_bom_master, has_crlf_master)





    # for i, h_file in enumerate(all_h_files):
    #     print("Processing {} {}/{}".format(h_file, i, len(all_h_files)))
    #     # move_trivial_destructors_to_h(cpp_file, all_h_files)
    #     remove_forward_class_decls(h_file, classes_per_header)


if __name__ == "__main__":
    main()

