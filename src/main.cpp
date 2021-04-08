#include <iostream>
#include <streambuf>
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include "XBindVisitor.h"

class XBindConsumer : public clang::ASTConsumer {
public:
    explicit XBindConsumer(clang::ASTContext *context) : visitor(context) {}

    void HandleTranslationUnit(clang::ASTContext &context) override {
        visitor.TraverseTranslationUnitDecl(context.getTranslationUnitDecl());
        const auto &decls = visitor.getTopLevelDecls();
        for (const auto &decl : decls) {
            // 此处设置输出流
            decl->emit(std::cout, 0);
        }
    }

private:
    XBindVisitor visitor;
};

class XBindAction : public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef inFile) override {
        return std::unique_ptr<clang::ASTConsumer>(new XBindConsumer(&compiler.getASTContext()));
    }
};

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string filename = argv[1];
        if (!llvm::sys::fs::exists(filename)) {
            llvm::errs() << "至少需要一个参数";
            return 1;
        }
        std::ifstream fin(filename);
        std::string code{std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>()};
        clang::tooling::runToolOnCode(std::make_unique<XBindAction>(), code);
    }
}
