/** ***************************************************************************
* @remark CS460: Programming Assignment 4 - Symbol Table                      *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  SymbolTable.h                                                       *
 * @date  April 8, 2025                                                       *
 *****************************************************************************/

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Symbol.h"
#include "Node.h"

class SymbolTable {
    struct SymbolNode {
        Symbol symbol;
        SymbolNode* next;
        SymbolNode(const Symbol& symbol) : symbol(symbol), next(nullptr) {}
    };
    SymbolNode* head;
    SymbolNode* tail;

    public:
    SymbolTable() : head(nullptr), tail(nullptr) {}
    ~SymbolTable();

    bool parameterExistsInScope(const string &name, int scope);

    bool addSymbol(const Symbol& symbol);
    Symbol* lookup(const string& name, int scope);
    void print();
};

void traverseCST(Node* node, int curScope, SymbolTable& symbolTable);
bool isValidIdentifier(const string& identifier);
extern bool symbolError;

#endif //SYMBOLTABLE_H
