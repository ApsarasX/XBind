#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>

#include "XBindType.h"

XBindType::XBindType(const clang::QualType &type) : type(type) {
    if (type->isBuiltinType()) {
        if (type->isVoidType()) {
            string = "void";
        } else if (type->isBooleanType()) {
            string = "boolean";
        } else if (type->isFixedPointOrIntegerType() || type->isFloatingType()) {
            string = "number";
        } else {
            // TODO: handle std::string
            string = type.getAsString();
        }
    } else if (type->isRecordType()) {
        for (const char c : type.getAsString()) {
            if (c == ':') {
                if (string.back() != '.') {
                    string.push_back('.');
                }
            } else {
                string.push_back(c);
            }
        }
    }
}

std::string XBindType::getString() const {
    return string;
}