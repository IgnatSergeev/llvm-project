//===--- RefactoringActions.cpp - Constructs refactoring actions ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"
#include "clang/Tooling/Refactoring/Edit/EditMatchRule.h"
#include "clang/Tooling/Refactoring/Extract/Extract.h"
#include "clang/Tooling/Refactoring/RefactoringAction.h"
#include "clang/Tooling/Refactoring/RefactoringOptions.h"
#include "clang/Tooling/Refactoring/Rename/RenamingAction.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include "clang/Tooling/Transformer/Stencil.h"

namespace clang {
namespace tooling {

using namespace transformer;
using namespace ast_matchers;

namespace {

class DeclNameOption final : public OptionalRefactoringOption<std::string> {
public:
  StringRef getName() const override { return "name"; }
  StringRef getDescription() const override {
    return "Name of the extracted declaration";
  }
};

// FIXME: Rewrite the Actions to avoid duplication of descriptions/names with
// rules.
class ExtractRefactoring final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "extract"; }

  StringRef getDescription() const override {
    return "(WIP action; use with caution!) Extracts code into a new function";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<ExtractFunction>(
        CodeRangeASTSelectionRequirement(),
        OptionRequirement<DeclNameOption>()));
    return Rules;
  }
};

class OldQualifiedNameOption : public RequiredRefactoringOption<std::string> {
public:
  StringRef getName() const override { return "old-qualified-name"; }
  StringRef getDescription() const override {
    return "The old qualified name to be renamed";
  }
};

class NewQualifiedNameOption : public RequiredRefactoringOption<std::string> {
public:
  StringRef getName() const override { return "new-qualified-name"; }
  StringRef getDescription() const override {
    return "The new qualified name to change the symbol to";
  }
};

class NewNameOption : public RequiredRefactoringOption<std::string> {
public:
  StringRef getName() const override { return "new-name"; }
  StringRef getDescription() const override {
    return "The new name to change the symbol to";
  }
};

// FIXME: Rewrite the Actions to avoid duplication of descriptions/names with
// rules.
class LocalRename final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "local-rename"; }

  StringRef getDescription() const override {
    return "Finds and renames symbols in code with no indexer support";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<RenameOccurrences>(
        SourceRangeSelectionRequirement(), OptionRequirement<NewNameOption>()));
    // FIXME: Use NewNameOption.
    Rules.push_back(createRefactoringActionRule<QualifiedRenameRule>(
        OptionRequirement<OldQualifiedNameOption>(),
        OptionRequirement<NewQualifiedNameOption>()));
    return Rules;
  }
};

class RedundandIf final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "redundand-if"; }

  StringRef getDescription() const override {
    return "Transforms if statement around given location with always true or "
           "false condition";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<EditMatchRule>(
        ASTLocMatchRequirement<StatementMatcher>(ifStmt(
            allOf(anyOf(hasCondition(cxxBoolLiteral(equals(true)).bind("true")),
                        hasCondition(cxxBoolLiteral(equals(false)))),
                  optionally(hasThen(stmt().bind("then"))),
                  optionally(hasElse(stmt().bind("else")))))),
        EditGeneratorRequirement(ifBound(
            "true", ifBound("then", changeTo(cat(node("then")))),
            ifBound("else", changeTo(cat(node("else"))), changeTo(cat("")))))));

    return Rules;
  }
};

class RedundandWhile final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "redundand-while"; }

  StringRef getDescription() const override {
    return "Removes while statement around given location with always false "
           "condition";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<EditMatchRule>(
        ASTLocMatchRequirement<StatementMatcher>(
            whileStmt(hasCondition(cxxBoolLiteral(equals(false))))),
        EditGeneratorRequirement(edit(changeTo(cat(""))))));

    return Rules;
  }
};

AST_MATCHER(Decl, isReferenced) { return Node.isReferenced(); }

class UnusedField final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "unused-field"; }

  StringRef getDescription() const override {
    return "Removes unused field of a class around given location";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<EditMatchRule>(
        ASTLocMatchRequirement<DeclarationMatcher>(
            fieldDecl(unless(isReferenced()),
                      unless(hasAttr(attr::Kind::Unused)))
                .bind("field")),
        EditGeneratorRequirement(edit(changeTo(node("field"), cat(""))))));

    return Rules;
  }
};

class UnusedMethod final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "unused-method"; }

  StringRef getDescription() const override {
    return "Removes unused method of a class around given location";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<EditMatchRule>(
        ASTLocMatchRequirement<DeclarationMatcher>(
            cxxMethodDecl(unless(isReferenced()),
                          unless(hasAttr(attr::Kind::Unused)))
                .bind("method")),
        EditGeneratorRequirement(edit(changeTo(node("method"), cat(""))))));

    return Rules;
  }
};

class SimplifyExpr final : public RefactoringAction {
public:
  StringRef getCommand() const override { return "simplify-expr"; }

  StringRef getDescription() const override {
    return "Simplifies expr around given location";
  }

  /// Returns a set of refactoring actions rules that are defined by this
  /// action.
  RefactoringActionRules createActionRules() const override {
    RefactoringActionRules Rules;
    Rules.push_back(createRefactoringActionRule<EditMatchRule>(
        ASTLocMatchRequirement<StatementMatcher>(anyOf(
            binaryOperator(
                hasOperatorName("&&"), hasRHS(expr().bind("rhs")),
                hasLHS(expr().bind("lhs")),
                anyOf(hasRHS(cxxBoolLiteral(equals(false)).bind("save-rhs")),
                      hasLHS(cxxBoolLiteral(equals(false))),
                      hasRHS(cxxBoolLiteral(equals(true))),
                      hasLHS(cxxBoolLiteral(equals(true)).bind("save-rhs")))),
            binaryOperator(
                hasOperatorName("||"), hasRHS(expr().bind("rhs")),
                hasLHS(expr().bind("lhs")),
                anyOf(hasRHS(cxxBoolLiteral(equals(false))),
                      hasLHS(cxxBoolLiteral(equals(false)).bind("save-rhs")),
                      hasRHS(cxxBoolLiteral(equals(true)).bind("save-rhs")),
                      hasLHS(cxxBoolLiteral(equals(true))))))),
        EditGeneratorRequirement(ifBound("save-rhs", changeTo(cat(node("rhs"))),
                                         changeTo(cat(node("lhs")))))));

    return Rules;
  }
};

} // end anonymous namespace

std::vector<std::unique_ptr<RefactoringAction>> createRefactoringActions() {
  std::vector<std::unique_ptr<RefactoringAction>> Actions;

  Actions.push_back(std::make_unique<LocalRename>());
  Actions.push_back(std::make_unique<ExtractRefactoring>());
  Actions.push_back(std::make_unique<RedundandIf>());
  Actions.push_back(std::make_unique<RedundandWhile>());
  Actions.push_back(std::make_unique<UnusedField>());
  Actions.push_back(std::make_unique<UnusedMethod>());
  Actions.push_back(std::make_unique<SimplifyExpr>());

  return Actions;
}

RefactoringActionRules RefactoringAction::createActiveActionRules() {
  // FIXME: Filter out rules that are not supported by a particular client.
  return createActionRules();
}

} // end namespace tooling
} // end namespace clang
