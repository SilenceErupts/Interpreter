
#ifndef INTERPRETER_AST_HPP
#define INTERPRETER_AST_HPP

/******************************************************************************
* @remark CS460: Programming Assignment 5 - Abstract Syntax Tree              *
 *                                                                            *
 * @author Shelby Anderson, Emilio Orozco, Jonathan Ramirez, Sam Tyler        *
 * @file  AST.hpp                                                             *
 * @date  April 22, 2025                                                      *
 *****************************************************************************/

#include "Parser.h"
#include "ASTNode.hpp"
#include "SymbolTable.h"

#include <vector>
#include <string>
#include <stack>

class AST {
public:
    AST(Node* CST, SymbolTable* symbolTable);
    ASTNode* root() const { return _ast; }
    void printASTWithSymbols(ASTNode *node);
    void printAST(ASTNode *node);

private:
    void buildAST();

    //buildAST Helpers
    ASTNode* lastRightSibling(ASTNode* node);
    void append(ASTNode*& node, ASTNode* newNode);

    //Creates each branch in the AST as a function
    ASTNode* createFuncProcDeclaration (Node* &CST);
    vector<ASTNode*> createVarDeclaration (Node* &CST);
    ASTNode* createAssignment(Node* &CST);
    ASTNode* createIf(Node* &CST);
    ASTNode* createElse(Node* &CST);
    ASTNode* createWhile(Node* &CST);
    ASTNode* createFor(Node* &CST);
    ASTNode* createCall(Node *&CST);
    ASTNode* createReturn(Node* &CST);
    ASTNode* createPrintf(Node* &CST);
    ASTNode* createBeginBlock(Node* &CST);
    ASTNode* createEndBlock(Node* &CST);

    //Get next token from CST
    Node* grabNext(Node* CST);

    // Expression to Postfix
    ASTNode* infixToPostfixNumerical(Node*& CST, bool stopOnSemi);
    ASTNode* infixToPostfixBoolean(Node*& CST, bool stopOnSemi);

    // Post-fix Helpers:
    int precedenceNumerical(const string &operation);
    int precedenceBoolean(const string &operation);
    bool isNumericalOperator(const string &token);
    bool isBooleanOperator(const string &token);

  Node* _cst;
  SymbolTable* _symbolTable;
  ASTNode* _ast;

  int curScope;
  int nextScopeId;
  int braceDepth;
};

#endif //INTERPRETER_AST_HPP