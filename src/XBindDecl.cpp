#include "XBindDecl.h"

///////////////////////////// XBindDecl /////////////////////////////

XBindDecl::Kind XBindDecl::getKind() const {
    return kind;
}

void XBindDecl::setKind(XBindDecl::Kind _kind) {
    kind = _kind;
}

const std::string &XBindDecl::getName() const {
    return name;
}

void XBindDecl::setName(const std::string &_name) {
    name = _name;
}

XBindDecl *XBindDecl::getParent() const {
    return parent;
}

void XBindDecl::setParent(XBindDecl *_parent) {
    parent = _parent;
}

const std::vector<XBindDecl *> &XBindDecl::getChildren() const {
    return children;
}

void XBindDecl::addChild(XBindDecl *child) {
    children.push_back(child);
}

void XBindDecl::setChildren(const std::vector<XBindDecl *> &_children) {
    children = _children;
}

bool XBindDecl::isNamespace() const {
    return kind == Kind::Namespace;
}

bool XBindDecl::isClass() const {
    return kind == Kind::Class;
}

bool XBindDecl::isField() const {
    return kind == Kind::DataMember;
}

bool XBindDecl::isMethod() const {
    return kind == Kind::MethodMember;
}

bool XBindDecl::isFunction() const {
    return kind == Kind::Function;
}

///////////////////////////// XBindNamespaceDecl /////////////////////////////

XBindNamespaceDecl::XBindNamespaceDecl() {
    kind = Kind::Namespace;
}

bool XBindNamespaceDecl::isAnonymous() const {
    return name.empty();
}

void XBindNamespaceDecl::emit(std::ostream &out, unsigned depth) {
    if (depth == 0) {
        out << "declare ";
    } else {
        out << std::string(depth * 4, ' ');
    }
    out << "namespace " << name << " {\n";
    for (auto child : children) {
        child->emit(out, depth + 1);
    }
    out << std::string(depth * 4, ' ') << "}\n";
}

///////////////////////////// XBindClassDecl /////////////////////////////

XBindClassDecl::XBindClassDecl() {
    kind = Kind::Class;
}

XBindType *XBindClassDecl::getSuperClassType() const {
    return superClassType;
}

void XBindClassDecl::setSuperClassType(XBindType *_superClassType) {
    superClassType = _superClassType;
}

void XBindClassDecl::emit(std::ostream &out, unsigned depth) {
    if (depth == 0) {
        out << "declare ";
    } else {
        out << std::string(depth * 4, ' ');
    }
    out << "class " << name;
    if (superClassType) {
        out << " extends " << superClassType->getString();
    }
    out << " {\n";
    for (auto child : children) {
        child->emit(out, depth + 1);
    }
    out << std::string(depth * 4, ' ') << "}\n";
}

///////////////////////////// XBindConstructorDecl /////////////////////////////

XBindConstructorDecl::XBindConstructorDecl() {
    kind = Kind::Constructor;
}

const std::vector<std::pair<std::string, XBindType *>> &XBindConstructorDecl::getParams() const {
    return params;
}

void XBindConstructorDecl::setParams(const std::vector<std::pair<std::string, XBindType *>> &_params) {
    params = _params;
}

void XBindConstructorDecl::addParam(const std::string &name, XBindType *type) {
    params.emplace_back(name, type);
}

void XBindConstructorDecl::emit(std::ostream &out, unsigned depth) {
    out << std::string(depth * 4, ' ');
    out << "constructor(";
    for (const auto &param : params) {
        out << param.first << ": " << param.second->getString() << ", ";
    }
    if (!params.empty()) {
        out << "\b\b";
    }
    out << ");\n";
}

///////////////////////////// XBindMemberDecl /////////////////////////////

XBindMemberDecl::XBindMemberDecl() {}

clang::AccessSpecifier XBindMemberDecl::getAccess() const {
    return access;
}

void XBindMemberDecl::setAccess(clang::AccessSpecifier _access) {
    access = _access;
}

bool XBindMemberDecl::isStatic() const {
    return _isStatic;
}

void XBindMemberDecl::setStatic(bool isStatic) {
    _isStatic = isStatic;
}

std::string XBindMemberDecl::getAccessString() const {
    switch (access) {
        case clang::AccessSpecifier::AS_public:
            return "public";
        case clang::AccessSpecifier::AS_protected:
            return "protected";
        case clang::AccessSpecifier::AS_private:
            return "private";
        case clang::AccessSpecifier::AS_none:
            return "";
    }
}

///////////////////////////// XBindDataMemberDecl /////////////////////////////

XBindDataMemberDecl::XBindDataMemberDecl() {
    kind = Kind::DataMember;
}

XBindType *XBindDataMemberDecl::getType() const {
    return type;
}

void XBindDataMemberDecl::setType(XBindType *_type) {
    type = _type;
}

bool XBindDataMemberDecl::isReadonly() const {
    return readonly;
}

void XBindDataMemberDecl::setReadonly(bool _readonly) {
    readonly = _readonly;
}

void XBindDataMemberDecl::emit(std::ostream &out, unsigned depth) {
    out << std::string(depth * 4, ' ');
    if (access != clang::AccessSpecifier::AS_none) {
        out << getAccessString() << ' ';
    }
    if (isStatic()) {
        out << "static ";
    }
    if (readonly) {
        out << "readonly ";
    }
    out << name << ": " << type->getString() << ";\n";
}

///////////////////////////// XBindMethodMemberDecl /////////////////////////////

XBindMethodMemberDecl::XBindMethodMemberDecl() {
    kind = Kind::MethodMember;
}

XBindType *XBindMethodMemberDecl::getReturnType() const {
    return returnType;
}

void XBindMethodMemberDecl::setReturnType(XBindType *_returnType) {
    returnType = _returnType;
}

const std::vector<std::pair<std::string, XBindType *>> &XBindMethodMemberDecl::getParams() const {
    return params;
}

void XBindMethodMemberDecl::setParams(const std::vector<std::pair<std::string, XBindType *>> &_params) {
    params = _params;
}

void XBindMethodMemberDecl::addParam(const std::string &name, XBindType *type) {
    params.emplace_back(name, type);
}

void XBindMethodMemberDecl::emit(std::ostream &out, unsigned depth) {
    out << std::string(depth * 4, ' ');
    if (access != clang::AccessSpecifier::AS_none) {
        out << getAccessString() << ' ';
    }
    if (isStatic()) {
        out << "static ";
    }
    out << name << "(";
    for (const auto &param : params) {
        out << param.first << ": " << param.second->getString() << ", ";
    }
    if (!params.empty()) {
        out << "\b\b";
    }
    out << "): " << returnType->getString() << ";\n";
}

///////////////////////////// XBindFunction /////////////////////////////

XBindVariableDecl::XBindVariableDecl() {
    kind = Kind::Variable;
}

bool XBindVariableDecl::isConstant() const {
    return _isConstant;
}

void XBindVariableDecl::setConstant(bool isConstant) {
    _isConstant = isConstant;
}

void XBindVariableDecl::emit(std::ostream &out, unsigned int depth) {
    if (depth == 0) {
        out << "declare ";
    } else {
        out << std::string(depth * 4, ' ');
    }
    out << (_isConstant ? "const " : "let ");
    out << name << ": " << type->getString() << ";\n";
}

XBindType *XBindVariableDecl::getType() const {
    return type;
}

void XBindVariableDecl::setType(XBindType *_type) {
    type = _type;
}

///////////////////////////// XBindFunction /////////////////////////////

XBindFunctionDecl::XBindFunctionDecl() {
    kind = Kind::Function;
}

XBindType *XBindFunctionDecl::getReturnType() const {
    return returnType;
}

void XBindFunctionDecl::setReturnType(XBindType *_returnType) {
    returnType = _returnType;
}

const std::vector<std::pair<std::string, XBindType *>> &XBindFunctionDecl::getParams() const {
    return params;
}

void XBindFunctionDecl::setParams(const std::vector<std::pair<std::string, XBindType *>> &_params) {
    params = _params;
}

void XBindFunctionDecl::addParam(const std::string &name, XBindType *type) {
    params.emplace_back(name, type);
}

void XBindFunctionDecl::emit(std::ostream &out, unsigned depth) {
    if (depth == 0) {
        out << "declare ";
    } else {
        out << std::string(depth * 4, ' ');
    }
    out << "function " << name << "(";
    for (const auto &param : params) {
        out << param.first << ": " << param.second->getString() << ", ";
    }
    if (!params.empty()) {
        out << "\b\b";
    }
    out << "): " << returnType->getString() << ";\n";
}
