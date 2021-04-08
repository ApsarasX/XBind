#pragma once

#include <string>
#include <clang/AST/Type.h>

class XBindType {
public:
    explicit XBindType(const clang::QualType &type);

    std::string getString() const;

private:
    clang::QualType type;
    std::string string;
};