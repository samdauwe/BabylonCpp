from types import *

def ToCamelCase(snake_case: str) -> str:
    items  = snake_case.split("_")
    def UpperFirstLetter(word):
        if len(word) == 0:
            return ""
        return word[:1].upper() + word[1:]    
    itemsCamel = map(UpperFirstLetter, items)
    r = "".join(itemsCamel)
    return r


def to_snake_case(CamelCase: str) -> str:
    r = ""
    if CamelCase[0] == "I" and CamelCase[1].isupper():
        r = "i" + CamelCase[1].lower()
        CamelCase = CamelCase[2:]
    else:
        r = CamelCase[0].lower()
        CamelCase = CamelCase[1:]

    # PBRMetallicRoughnessMaterial
    for i in range(len(CamelCase)):
        c = CamelCase[i]
        add_underscore = False
        if c.isupper() and i < len(CamelCase) - 1 and CamelCase[i + 1].islower():
            add_underscore = True
        if add_underscore:
            r = r + "_"            
        r = r + c.lower()
    return r

# print(to_snake_case("HelloWorld"))
# print(to_snake_case("PBRMetallicRoughnessMaterial"))
# print(to_snake_case("ICanvas"))
