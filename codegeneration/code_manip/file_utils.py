import os
import os.path
import codecs
from typing import *
from bab_types import *


def files_with_extension(folder: Folder, extension: Extension) -> List[FileFullPath]:
    r = []
    ext_len = len(extension)
    if not os.path.isdir(folder):
        print("ouch")
    for root, _, files in os.walk(folder):
        for file in files:
            if file[-ext_len:] == extension:
                full_file = root + "/" + file
                full_file = full_file.replace("\\", "/")
                r.append(full_file)
    return r


def file_has_utf8_bom(file: FileFullPath) -> bool:
    bom_marker = codecs.BOM_UTF8
    with open(file, "rb") as f:
        content = f.read()
        start = content[:3]
        if start == bom_marker: #u'\ufeff':
            return True
        return False


def file_has_windows_crlf(file: FileFullPath) -> bool:
    with open(file, "rb") as f:
        content = f.read()
    nb_lf = 0
    nb_crlf = 0
    was_last_char_cr = False
    for i, b in enumerate(content):
        if b == b'\n'[0]:
            if not was_last_char_cr:
                nb_lf = nb_lf + 1
        if b == b'\r'[0]:
            nb_crlf = nb_crlf + 1
            was_last_char_cr = True
        else:
            was_last_char_cr = False

    if nb_lf > 0 and nb_crlf > 0:
        raise Exception("Mixed CR CRLF!")
    return nb_crlf > nb_lf

def read_file_lines_no_eol(file_full_path: FileFullPath) -> List[CodeLine]:
    with open(file_full_path, "r") as f:
        content = f.read()
    lines = content.split("\n")
    return lines


def write_file_lines_no_eol(file_full_path: FileFullPath, lines: List[CodeLine]):
    content = "\n".join(lines)
    with open(file_full_path, "w") as f:
        f.write(content)


def write_file_lines_no_eol_formatted(
    file: FileFullPath, 
    lines: List[CodeLine], 
    has_utf8_bom: bool,
    has_windows_crlf: bool
    ):
    bom_marker = codecs.BOM_UTF8
    if has_windows_crlf:
        content = "\r\n".join(lines)
    else:
        content ="\n".join(lines)
    with open(file, "wb") as f:
        if has_utf8_bom:
            f.write(bom_marker)
        bytes_content = content.encode('utf-8')
        f.write(bytes_content)

def is_cpp(file: FileFullPath) -> bool:
    return file.endswith(".cpp")


def h_file_from_cpp(cpp_file: FileFullPath, all_h_files: List[FileFullPath]) -> Optional[FileFullPath]:    
    items = cpp_file.split("/")
    file_with_parent_folder = "/".join(items[-2:])
    basename_with_parent_folder = file_with_parent_folder.replace(".cpp", "")
    found_h_files = list(filter(lambda f: basename_with_parent_folder + ".h" in f, all_h_files))
    assert(len(found_h_files) <= 1)
    if len(found_h_files) == 1:
        return found_h_files[0]
    else:
        return None


def make_babylon_include_path(h_file: FileFullPath):
    include = h_file
    idx = include.find("include/babylon")
    if idx < 0:
        return None
    include = include[idx + 8:]
    return include
