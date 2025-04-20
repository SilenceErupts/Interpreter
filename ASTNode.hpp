#ifndef INTERPRETER_ASTNODE_HPP
#define INTERPRETER_ASTNODE_HPP

/******************************************************************************
* @remark CS460: Programming Assignment 5 - Abstract Syntax Tree              *
 *                                                                            *
 * @author Shelby Anderson, Emilio Orozco, Jonathan Ramirez, Sam Tyler        *
 * @file  ASTNode.cpp                                                         *
 * @date  April 22, 2025                                                      *
 *****************************************************************************/

#include <string>
#include <iostream>

#include "SymbolTable.h"

using namespace std;

struct ASTNode {
    string name;
    int lineNumber;
    ASTNode* leftChild;
    ASTNode* rightSibling;
    Symbol* symbol;

    ASTNode(const std::string &n, int line = 0)
            : name(n), lineNumber(line), leftChild(nullptr), rightSibling(nullptr), symbol(nullptr) {}
};

// Add a child node to LCRS tree
inline void addChild(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    while (parent->rightSibling) {
        parent = parent->rightSibling;
    }
    parent->leftChild = child;
}

// Add a sibling node to the LCRS tree
inline void addSibling(ASTNode* leftNode, ASTNode* sibling) {
    if (!leftNode || !sibling) return;
    while (leftNode->rightSibling) {
        leftNode = leftNode->rightSibling;
    }
    leftNode->rightSibling = sibling;
}

#endif //INTERPRETER_ASTNODE_HPP
