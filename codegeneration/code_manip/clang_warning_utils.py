from bab_types import *

def is_warning_trivial_destructor(line):
    return "trivial destructor" in line and "hicpp-use-equals-default" in line


def warning_location(warning_line) -> (FileFullPath, LineNumber):
    items = warning_line.split(" ")
    if ":" in items[0]:
        location = items[0]
    elif ":" in items[1]:
        location = items[1]
    else:
        raise Exception("Could not parse warning")
    items2 = location.split(":")
    file_full_path = items2[0]
    line_number = items2[1]
    return file_full_path, int(line_number) - 1

