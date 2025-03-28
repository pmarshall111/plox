# A script to create C++ structs used in an AST

import subprocess
from typing import TypedDict


class ClassMember(TypedDict):
    type: str
    name: str


class ClassDef(TypedDict):
    name: str
    members:list[ClassMember]


def define_ast(out_file: str, parent: str, namespaces: list[str], subclasses: list[ClassDef]) -> None:
    with open(out_file, "w") as file:
        # Warning
        file.write("// WARNING: DO NOT MANUALLY EDIT. This file is auto-generated by 'gen_ast_h.py'\n\n")

        # Header guard
        with HeaderGuard(file, "PLOX_AUTO_GENERATED_AST"):    

            # Includes
            file.write("#include <string>\n\n")
            file.write("#include <scanner.h>\n\n")

            # Namespaces
            with NamespaceGuard(file, namespaces):
                
                # Parent class
                file.write(f"struct {parent} {{}};\n")
        
                # Sub classes
                for subc in subclasses:
                    file.write(f"""

                    struct {subc["name"]}: {parent} {{
                        {"\n".join(f"\t{memb["type"]} {memb["name"]};" for memb in subc["members"])}
                    }};

                    """)

    # Format output
    subprocess.run(["make", "format"], check=True)



# Guards to centralise logic for lines that occur at both the start and end of the file
class HeaderGuard:
    def __init__(self, file, name):
        self.file = file
        self.name = name

    def __enter__(self):
        self.file.write(f"""#ifndef {self.name}
        #define {self.name}

        """)

    def __exit__(self, *_):
        self.file.write(f"""
        #endif // {self.name}
        """)


class NamespaceGuard:
    def __init__(self, file, namespaces):
        self.file = file
        self.namespaces = namespaces

    def __enter__(self):
        self.file.write("\n".join([f"namespace {ns} {{" for ns in self.namespaces]))
        self.file.write("\n\n")

    def __exit__(self, *_):
        self.file.write("\n".join([f"}} // namespace {ns}" for ns in self.namespaces]))
        self.file.write("\n\n")


if __name__ == "__main__":
    define_ast("tree-walk/src/ast.h", "Expr", ["plox", "treewalk", "ast"], [
        {"name": "Binary", "members": [{"type": "Expr", "name": "left"}, {"type": "Token", "name": "operator"}, {"type": "Expr", "name": "right"}]},
        {"name": "Grouping", "members": [{"type": "Expr", "name": "Expression"}]},
        {"name": "Literal", "members": [{"type": "Object", "name": "value"}]},
        {"name": "Unary", "members": [{"type": "Token", "name": "operator"}, {"type": "Expr", "name": "right"}]}
    ])
