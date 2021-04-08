#pragma once

#include <vector>
#include <stack>
#include <clang/AST/RecursiveASTVisitor.h>

#include "XBindDecl.h"

class XBindVisitor : public clang::RecursiveASTVisitor<XBindVisitor> {
public:
    explicit XBindVisitor(clang::ASTContext *context) : context(context) {}

    bool TraverseTranslationUnitDecl(clang::TranslationUnitDecl *unitDecl);

    bool VisitTranslationUnitDecl(clang::TranslationUnitDecl *unitDecl);

    bool VisitNamespaceDecl(clang::NamespaceDecl *namespaceDecl);

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *recordDecl);

    bool VisitCXXConstructorDecl(clang::CXXConstructorDecl *ctorDecl);

    bool VisitFieldDecl(clang::FieldDecl *fieldDecl);

    bool VisitVarDecl(clang::VarDecl *varDecl);

    bool VisitCXXMethodDecl(clang::CXXMethodDecl *decl);

    bool VisitFunctionDecl(clang::FunctionDecl *decl);

    const std::vector<XBindDecl *> &getTopLevelDecls() const;

private:
    clang::ASTContext *context;
    std::stack<XBindDecl *> declStack;
    std::vector<XBindDecl *> topLevelDecls;
};