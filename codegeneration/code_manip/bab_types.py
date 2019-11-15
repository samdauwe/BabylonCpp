"""
Type synonyms for code clarity
"""
CodeLine = str
CompilerLine = str
Folder = str
Extension = str
FileFullPath = str
FileBasename = str
FileIncludePath = str
LineNumber = int
ClassName = str

def unique(list1): 
    unique_list = [] 
    for x in list1: 
        if x not in unique_list: 
            unique_list.append(x) 
    return  unique_list
