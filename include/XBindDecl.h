#pragma once

#include <fstream>
#include <vector>
#include <utility>
#include <clang/AST/Decl.h>

#include "XBindType.h"

class XBindDecl {
public:
    enum class Kind {
        Namespace,
        Class,
        Constructor,
        DataMember,
        MethodMember,
        Variable,
        Function,
    };

    virtual Kind getKind() const;

    virtual void setKind(Kind _kind);

    virtual const std::string &getName() const;

    virtual void setName(const std::string &_name);

    virtual XBindDecl *getParent() const;

    virtual void setParent(XBindDecl *_parent);

    virtual const std::vector<XBindDecl *> &getChildren() const;

    virtual void addChild(XBindDecl *child);

    virtual void setChildren(const std::vector<XBindDecl *> &_children);

    virtual bool isNamespace() const;

    virtual bool isClass() const;

    virtual bool isField() const;

    virtual bool isMethod() const;

    virtual bool isFunction() const;

    virtual void emit(std::ostream &out, unsigned depth) = 0;

protected:
    Kind kind;
    std::string name;
    XBindDecl *parent = nullptr;
    std::vector<XBindDecl *> children;
};

// 命名空间
class XBindNamespaceDecl : public XBindDecl {
public:
    explicit XBindNamespaceDecl();

    void emit(std::ostream &out, unsigned depth) override;

    bool isAnonymous() const;
};

// 类(包括C++类和结构体)
class XBindClassDecl : public XBindDecl {
public:
    explicit XBindClassDecl();

    XBindType *getSuperClassType() const;

    void setSuperClassType(XBindType *_superClassType);

    void emit(std::ostream &out, unsigned depth) override;

private:
    XBindType *superClassType = nullptr;
};

// 类的构造函数
class XBindConstructorDecl : public XBindDecl {
public:
    explicit XBindConstructorDecl();

    const std::vector<std::pair<std::string, XBindType *>> &getParams() const;

    void setParams(const std::vector<std::pair<std::string, XBindType *>> &_params);

    void addParam(const std::string &name, XBindType *type);

    void emit(std::ostream &out, unsigned depth) override;

protected:
    std::vector<std::pair<std::string, XBindType *>> params;
};

// 类成员
class XBindMemberDecl : public XBindDecl {
public:
    explicit XBindMemberDecl();

    clang::AccessSpecifier getAccess() const;

    void setAccess(clang::AccessSpecifier access);

    bool isStatic() const;

    void setStatic(bool isStatic);

protected:
    std::string getAccessString() const;

    clang::AccessSpecifier access = clang::AccessSpecifier::AS_none;

    bool _isStatic = false;
};

// 类数据成员
class XBindDataMemberDecl : public XBindMemberDecl {
public:
    explicit XBindDataMemberDecl();

    XBindType *getType() const;

    void setType(XBindType *_type);

    bool isReadonly() const;

    void setReadonly(bool _readonly);

    void emit(std::ostream &out, unsigned depth) override;

protected:
    XBindType *type = nullptr;

    bool readonly = false;
};

// 类方法成员
class XBindMethodMemberDecl : public XBindMemberDecl {
public:
    explicit XBindMethodMemberDecl();

    XBindType *getReturnType() const;

    void setReturnType(XBindType *returnType);

    const std::vector<std::pair<std::string, XBindType *>> &getParams() const;

    void setParams(const std::vector<std::pair<std::string, XBindType *>> &params);

    void addParam(const std::string &name, XBindType *type);

    void emit(std::ostream &out, unsigned depth) override;

protected:
    XBindType *returnType = nullptr;
    std::vector<std::pair<std::string, XBindType *>> params;
};


// 变量
class XBindVariableDecl : public XBindDecl {
public:
    explicit XBindVariableDecl();

    bool isConstant() const;

    void setConstant(bool _isConstant);

    XBindType *getType() const;

    void setType(XBindType *type);

    void emit(std::ostream &out, unsigned int depth) override;

protected:
    bool _isConstant = false;
    XBindType *type = nullptr;
};

// 函数
class XBindFunctionDecl : public XBindDecl {
public:
    explicit XBindFunctionDecl();

    XBindType *getReturnType() const;

    void setReturnType(XBindType *returnType);

    const std::vector<std::pair<std::string, XBindType *>> &getParams() const;

    void setParams(const std::vector<std::pair<std::string, XBindType *>> &params);

    void addParam(const std::string &name, XBindType *type);

    void emit(std::ostream &out, unsigned depth) override;

protected:
    XBindType *returnType = nullptr;
    std::vector<std::pair<std::string, XBindType *>> params;
};