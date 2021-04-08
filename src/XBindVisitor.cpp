#include <iostream>
#include "XBindVisitor.h"

bool XBindVisitor::TraverseTranslationUnitDecl(clang::TranslationUnitDecl *unitDecl) {
    VisitTranslationUnitDecl(unitDecl);
    return true;
}

bool XBindVisitor::VisitTranslationUnitDecl(clang::TranslationUnitDecl *unitDecl) {
    for (const auto &decl : unitDecl->decls()) {
        if (clang::NamespaceDecl::classof(decl)) {
            VisitNamespaceDecl(static_cast<clang::NamespaceDecl *>(decl));
        } else if (clang::CXXRecordDecl::classof(decl)) {
            VisitCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(decl));
        } else if (clang::FunctionDecl::classof(decl)) {
            VisitFunctionDecl(static_cast<clang::FunctionDecl *>(decl));
        } else if (clang::VarDecl::classof(decl)) {
            VisitVarDecl(static_cast<clang::VarDecl *>(decl));
        }
    }
    return true;
}

bool XBindVisitor::VisitNamespaceDecl(clang::NamespaceDecl *namespaceDecl) {
    auto *xbindDecl = new XBindNamespaceDecl();
    xbindDecl->setName(namespaceDecl->getNameAsString());
    if (!declStack.empty()) {
        xbindDecl->setParent(declStack.top());
        declStack.top()->addChild(xbindDecl);
    }
    declStack.push(xbindDecl);
    for (const auto &decl : namespaceDecl->decls()) {
        if (clang::CXXRecordDecl::classof(decl)) {
            VisitCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(decl));
        } else if (clang::FunctionDecl::classof(decl)) {
            VisitFunctionDecl(static_cast<clang::FunctionDecl *>(decl));
        } else if (clang::VarDecl::classof(decl)) {
            VisitVarDecl(static_cast<clang::VarDecl *>(decl));
        }
    }
    declStack.pop();
    if (declStack.empty()) {
        topLevelDecls.push_back(xbindDecl);
    }
    return true;
}

bool XBindVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *recordDecl) {
    auto *xbindDecl = new XBindClassDecl();
    xbindDecl->setName(recordDecl->getNameAsString());
    if (recordDecl->getNumBases() > 0) {
        const clang::QualType &baseType = recordDecl->bases_begin()->getType();
        xbindDecl->setSuperClassType(new XBindType(baseType));
    }
    if (!declStack.empty()) {
        xbindDecl->setParent(declStack.top());
        declStack.top()->addChild(xbindDecl);
    }
    declStack.push(xbindDecl);
    for (const auto &decl : recordDecl->decls()) {
        if (clang::CXXConstructorDecl::classof(decl)) {
            VisitCXXConstructorDecl(static_cast<clang::CXXConstructorDecl *>(decl));
        } else if (clang::FieldDecl::classof(decl)) {
            VisitFieldDecl(static_cast<clang::FieldDecl *>(decl));
        } else if (clang::VarDecl::classof(decl)) {
            VisitVarDecl(static_cast<clang::VarDecl *>(decl));
        } else if (clang::CXXMethodDecl::classof(decl)) {
            VisitCXXMethodDecl(static_cast<clang::CXXMethodDecl *>(decl));
        }
    }
    declStack.pop();
    if (declStack.empty()) {
        topLevelDecls.push_back(xbindDecl);
    }
    return true;
}

bool XBindVisitor::VisitCXXConstructorDecl(clang::CXXConstructorDecl *ctorDecl) {
    auto xbindDecl = new XBindConstructorDecl();
    xbindDecl->setParent(declStack.top());
    declStack.top()->addChild(xbindDecl);
    unsigned paramSize = ctorDecl->getNumParams();
    for (unsigned i = 0; i < paramSize; ++i) {
        auto *paramDecl = ctorDecl->getParamDecl(i);
        const std::string &paramName = paramDecl->getNameAsString();
        const clang::QualType &paramType = paramDecl->getType();
        xbindDecl->addParam(paramName, new XBindType(paramType));
    }
    return true;
}

// FieldDecl为类非静态成员变量
bool XBindVisitor::VisitFieldDecl(clang::FieldDecl *fieldDecl) {
    auto *xbindDecl = new XBindDataMemberDecl();
    xbindDecl->setParent(declStack.top());
    declStack.top()->addChild(xbindDecl);
    xbindDecl->setName(fieldDecl->getNameAsString());
    xbindDecl->setAccess(fieldDecl->getAccess());
    bool readonly = fieldDecl->getType().isConstant(*context);
    xbindDecl->setReadonly(readonly);
    const clang::QualType &type = fieldDecl->getType();
    xbindDecl->setType(new XBindType(type));
    return true;
}

// 这里的VarDecl是类静态成员变量或者普通变量
bool XBindVisitor::VisitVarDecl(clang::VarDecl *varDecl) {
    const clang::QualType &type = varDecl->getType();
    if (varDecl->isCXXClassMember()) {
        auto *xbindDecl = new XBindDataMemberDecl();
        xbindDecl->setParent(declStack.top());
        declStack.top()->addChild(xbindDecl);
        xbindDecl->setName(varDecl->getNameAsString());
        xbindDecl->setAccess(varDecl->getAccess());
        xbindDecl->setStatic(true);
        bool readonly = varDecl->getType().isConstant(*context);
        xbindDecl->setReadonly(readonly);
        xbindDecl->setType(new XBindType(type));
    } else {
        auto *xbindDecl = new XBindVariableDecl();
        if (!declStack.empty()) {
            xbindDecl->setParent(declStack.top());
            declStack.top()->addChild(xbindDecl);
        }
        xbindDecl->setName(varDecl->getNameAsString());
        xbindDecl->setType(new XBindType(type));
    }
    return true;
}


bool XBindVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *methodDecl) {
    if (methodDecl->isCopyAssignmentOperator() ||
        methodDecl->isMoveAssignmentOperator()) {
        return true;
    }
    auto *xbindDecl = new XBindMethodMemberDecl();
    xbindDecl->setParent(declStack.top());
    declStack.top()->addChild(xbindDecl);
    xbindDecl->setName(methodDecl->getNameAsString());
    xbindDecl->setAccess(methodDecl->getAccess());
    xbindDecl->setStatic(methodDecl->isStatic());
    unsigned paramSize = methodDecl->getNumParams();
    for (unsigned i = 0; i < paramSize; ++i) {
        auto *paramDecl = methodDecl->getParamDecl(i);
        const std::string &paramName = paramDecl->getNameAsString();
        const clang::QualType &paramType = paramDecl->getType();
        xbindDecl->addParam(paramName, new XBindType(paramType));
    }
    const clang::QualType &returnType = methodDecl->getReturnType();
    xbindDecl->setReturnType(new XBindType(returnType));
    return true;
}

bool XBindVisitor::VisitFunctionDecl(clang::FunctionDecl *functionDecl) {
    auto *xbindDecl = new XBindFunctionDecl();
    if (!declStack.empty()) {
        xbindDecl->setParent(declStack.top());
        declStack.top()->addChild(xbindDecl);
    }
    xbindDecl->setName(functionDecl->getNameAsString());
    unsigned paramSize = functionDecl->getNumParams();
    for (unsigned i = 0; i < paramSize; ++i) {
        auto *paramDecl = functionDecl->getParamDecl(i);
        const std::string &paramName = paramDecl->getNameAsString();
        const clang::QualType &paramType = paramDecl->getType();
        xbindDecl->addParam(paramName, new XBindType(paramType));
    }
    const clang::QualType &returnType = functionDecl->getReturnType();
    xbindDecl->setReturnType(new XBindType(returnType));
    if (declStack.empty()) {
        topLevelDecls.push_back(xbindDecl);
    }
    return true;
}

const std::vector<XBindDecl *> &XBindVisitor::getTopLevelDecls() const {
    return topLevelDecls;
}
