/** ***************************************************************************
* @remark CS460: Programming Assignment 4 - Symbol Table                      *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  SymbolTable.cpp                                                     *
 * @date  April 8, 2025                                                       *
 *****************************************************************************/

#include "SymbolTable.h"
#include <iostream>

using namespace std;

bool symbolError = false;
static int nextScope = 1;

SymbolTable::~SymbolTable() {
    while (head) {
        SymbolNode* temp = head;
        head = head->next;
        delete temp;
    }
}

bool SymbolTable::parameterExistsInScope(const string &name, int scope) {
    SymbolNode* cur = head;
    while (cur) {
        if ((cur->symbol.idType == "function" || cur->symbol.idType == "procedure") &&
            cur->symbol.scope == scope) {
            // Check if any parameter has the same name
            for (const auto &param : cur->symbol.parameters) {
                    if (param.name == name)
                        return true;
                }
            }
        cur = cur->next;
    }
    return false;
}

bool SymbolTable::addSymbol(const Symbol& symbol) {
    // check for a global duplicate
    if (symbol.idType == "datatype" && symbol.scope != 0) {
        if (lookup(symbol.name, 0) != nullptr) {
            return false;
        }
    }
    // check for duplicate in current scope
    if (lookup(symbol.name, symbol.scope) != nullptr) {
        return false;
    }
    // check for duplicates in the function/procedure parameter list
    if (symbol.idType == "datatype" && symbol.scope != 0) {
        if (parameterExistsInScope(symbol.name, symbol.scope)) {
            return false;
        }
    }
    SymbolNode* newNode = new SymbolNode(symbol);
    if (head == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    return true;
}

Symbol* SymbolTable::lookup(const string& name, int scope) {
    SymbolNode* cur = head;
    while (cur) {
        if (cur->symbol.name == name && cur->symbol.scope == scope) {
            return &cur->symbol;
        }
        cur = cur->next;
    }
    return nullptr;
}

void SymbolTable::print() {
    SymbolNode* cur = head;
    //  print all symbol table entries
    while (cur) {
        cout << "IDENTIFIER_NAME: " << cur->symbol.name << "\n";
        cout << "IDENTIFIER_TYPE: " << cur->symbol.idType << "\n";
        cout << "DATATYPE: " << cur->symbol.dataType << "\n";
        cout << "DATATYPE_IS_ARRAY: " << (cur->symbol.isArray ? "yes" : "no") << "\n";
        cout << "DATATYPE_ARRAY_SIZE: " << cur->symbol.arraySize << "\n";
        cout << "SCOPE: " << cur->symbol.scope << "\n\n";
        cur = cur->next;
    }
    // Then print parameter lists for entries
    cur = head;
    while (cur) {
        if ((cur->symbol.idType == "function" || cur->symbol.idType == "procedure") && !cur->symbol.parameters.empty()) {
            cout << "PARAMETER LIST FOR: " << cur->symbol.name << "\n";
            for (auto &p : cur->symbol.parameters) {
                cout << "IDENTIFIER_NAME: " << p.name << "\n";
                cout << "DATATYPE: " << p.dataType << "\n";
                cout << "DATATYPE_IS_ARRAY: " << (p.isArray ? "yes" : "no") << "\n";
                cout << "DATATYPE_ARRAY_SIZE: " << p.arraySize << "\n";
                cout << "SCOPE: " << p.scope << "\n\n";
                }
        }
        cur = cur->next;
    }
}

bool isValidIdentifier(const string &tokenText) {
    if (tokenText.empty()) {
        return false;
    }
    char c = tokenText[0];
    return isalpha(c) || (c == '_');
}

vector<Node*> flattenParameterNodes(Node* node) {
    vector<Node*> params;
    if (!node) {
        return params;
    }
    // If node is non-terminal derivation, process children
    if (node->name == "ParameterList" ||
        node->name == "IdentifierAndIdentifierArrayList" ||
        node->name == "IdentifierList" ||
        node->name == "IdentifierArrayList" ||
        node->name == "IdentifierAndIdentifierArrayParameterListDeclaration") {
            vector<Node*> childParams = flattenParameterNodes(node->leftChild);
            params.insert(params.end(), childParams.begin(), childParams.end());
    }
    // otherwise, make sure it is valid char, int, or bool
    else if (isValidIdentifier(node->name) ||
             node->name == "char" || node->name == "int" || node->name == "bool") {
                params.push_back(node);
             }
    // Process right siblings
    vector<Node*> siblingParams = flattenParameterNodes(node->rightSibling);
    params.insert(params.end(), siblingParams.begin(), siblingParams.end());
    return params;
}

void processIdentifierList(Node* node, int curScope, const string &datatype, SymbolTable &st) {
    if (!node) {
        return;
    }
    if (node->name == "IdentifierAndIdentifierArrayList" ||
        node->name == "IdentifierList" ||
        node->name == "IdentifierArrayList") {
        processIdentifierList(node->leftChild, curScope, datatype, st);
        }
    else if (isValidIdentifier(node->name)) {
        bool isArray = false;
        int arraySize = 0;
        if (node->rightSibling && node->rightSibling->name == "[") {
            Node* sizeNode = node->rightSibling->rightSibling;
            if (sizeNode && isdigit(sizeNode->name[0])) {
                arraySize = stoi(sizeNode->name);
                isArray = true;
            }
        }
        Symbol s { node->name, "datatype", datatype, curScope, isArray, arraySize, node->lineNumber };
        if (!st.addSymbol(s)) {
            symbolError = true;
            if (s.scope != 0 && st.lookup(s.name, 0) != nullptr) {
                cerr << "Error on line " << s.line << ": variable '" << s.name << "' is already defined globally\n";
            }
            else {
                cerr << "Error on line " << s.line << ": variable '" << s.name << "' is already defined locally\n";
            }
        }
    }
    processIdentifierList(node->rightSibling, curScope, datatype, st);
}

void traverseCST(Node* node, int curScope, SymbolTable& symbolTable) {
    if (!node) {
        return;
    }
    // begin DeclarationStatement
    if (node->name == "DeclarationStatement") {
        string datatype = (node->leftChild ? node->leftChild->name : "");
        processIdentifierList(node->leftChild ? node->leftChild->rightSibling : nullptr, curScope, datatype, symbolTable);
        traverseCST(node->rightSibling, curScope, symbolTable);
        return;
    }
    // begin FunctionDeclartion
    if (node->name == "FunctionDeclaration") {
        Node* retTypeNode = (node->leftChild ? node->leftChild->rightSibling : nullptr);
        Node* funcNameNode = (retTypeNode ? retTypeNode->rightSibling : nullptr);
        if (funcNameNode && isValidIdentifier(funcNameNode->name)) {
            string funcName = funcNameNode->name;
            int funcScope = nextScope++;
            Symbol funcSym;
            funcSym.name = funcName;
            funcSym.idType = "function";
            funcSym.dataType = (retTypeNode ? retTypeNode->name : "unknown");
            funcSym.scope = funcScope;
            funcSym.isArray = false;
            funcSym.arraySize = 0;
            funcSym.line = funcNameNode->lineNumber;
            if (!symbolTable.addSymbol(funcSym)) {
                cerr << "Error on line " << funcNameNode->lineNumber
                     << ": function '" << funcName << "' is already defined in scope " << funcScope << ".\n";
            }
            //Begin parameterList
            Node* paramList = nullptr;
            Node* search = node->leftChild;
            while (search) {
                if (search->name == "ParameterList") {
                    paramList = search;
                    break;
                }
                search = search->rightSibling;
            }
            if (paramList) {
                // Flatten the parameter list into a vector
                vector<Node*> realParams = flattenParameterNodes(paramList->leftChild);
                // pair datatypes with identifier
                for (int i = 0; i + 1 < realParams.size(); i += 2) {
                    Node* typeNode = realParams[i];
                    Node* nameNode = realParams[i+1];
                    // If the parameter node is non-terminal derivation, use leftChild
                    if (nameNode && (nameNode->name == "IdentifierAndIdentifierArrayParameterListDeclaration" ||
                                     nameNode->name == "IdentifierAndIdentifierArrayList")) {
                        nameNode = nameNode->leftChild;
                    }
                    bool isArray = false;
                    int arraySize = 0;
                    if (nameNode && nameNode->rightSibling && nameNode->rightSibling->name == "[") {
                        Node* sizeNode = nameNode->rightSibling->rightSibling;
                        if (sizeNode) {
                            arraySize = stoi(sizeNode->name);
                            isArray = true;
                        }
                    }
                    if (nameNode && isValidIdentifier(nameNode->name)) {
                        Symbol p;
                        p.name = nameNode->name;
                        p.idType = "datatype";
                        p.dataType = typeNode->name;
                        p.scope = funcScope;
                        p.isArray = isArray;
                        p.arraySize = arraySize;
                        p.line = nameNode->lineNumber;
                        Symbol* funcEntry = symbolTable.lookup(funcName, funcScope);
                        if (funcEntry) {
                            funcEntry->parameters.push_back(p);
                        }
                    }
                }
            }
            // function body
            Node* body = nullptr;
            search = node->leftChild;
            while (search) {
                if (search->name == "BlockStatement" || search->name == "CompoundStatement") {
                    body = search;
                    break;
                }
                search = search->rightSibling;
            }
            traverseCST(body, funcScope, symbolTable);
        }
        traverseCST(node->rightSibling, curScope, symbolTable);
        return;
    }
    // ProcedureDeclaration / MainProcedure
    if (node->name == "ProcedureDeclaration" || node->name == "MainProcedure") {
        if (node->leftChild) {
            Node* procNameNode = node->leftChild->rightSibling;
            if (procNameNode && isValidIdentifier(procNameNode->name)) {
                int procScope = nextScope++;
                string dtype = "NOT APPLICABLE";
                Symbol procSym;
                procSym.name = procNameNode->name;
                procSym.idType = "procedure";
                procSym.dataType = dtype;
                procSym.scope = procScope;
                procSym.isArray = false;
                procSym.arraySize = 0;
                procSym.line = procNameNode->lineNumber;
                if (!symbolTable.addSymbol(procSym)) {
                    cerr << "Error on line " << procNameNode->lineNumber
                         << ": procedure '" << procNameNode->name
                         << "' is already defined in scope " << procScope << ".\n";
                }
                // check ParameterList for procedure if it exists
                Node* paramList = nullptr;
                Node* searchP = node->leftChild;
                while (searchP) {
                    if (searchP->name == "ParameterList") {
                        paramList = searchP;
                        break;
                    }
                    searchP = searchP->rightSibling;
                }
                if (paramList) {
                    vector<Node*> realParams = flattenParameterNodes(paramList->leftChild);
                    for (int i = 0; i + 1 < realParams.size(); i += 2) {
                        Node* typeNode = realParams[i];
                        Node* nameNode = realParams[i+1];
                        if (nameNode && (nameNode->name == "IdentifierAndIdentifierArrayParameterListDeclaration" ||
                                         nameNode->name == "IdentifierAndIdentifierArrayList")) {
                            nameNode = nameNode->leftChild;
                        }
                        bool isArray = false;
                        int arraySize = 0;
                        if (nameNode && nameNode->rightSibling && nameNode->rightSibling->name == "[") {
                            Node* sizeNode = nameNode->rightSibling->rightSibling;
                            if (sizeNode) {
                                arraySize = stoi(sizeNode->name);
                                isArray = true;
                            }
                        }
                        if (nameNode && isValidIdentifier(nameNode->name)) {
                            Symbol p;
                            p.name = nameNode->name;
                            p.idType = "datatype";
                            p.dataType = typeNode->name;
                            p.scope = procScope;
                            p.isArray = isArray;
                            p.arraySize = arraySize;
                            p.line = nameNode->lineNumber;
                            Symbol* procEntry = symbolTable.lookup(procNameNode->name, procScope);
                            if (procEntry) {
                                procEntry->parameters.push_back(p);
                            }
                        }
                    }
                }
                // procedure body
                Node* body = nullptr;
                Node* search2 = node->leftChild;
                while (search2) {
                    if (search2->name == "BlockStatement" || search2->name == "CompoundStatement") {
                        body = search2;
                        break;
                    }
                    search2 = search2->rightSibling;
                }
                traverseCST(body, procScope, symbolTable);
            }
        }
        traverseCST(node->rightSibling, curScope, symbolTable);
        return;
    }
    traverseCST(node->leftChild, curScope, symbolTable);
    traverseCST(node->rightSibling, curScope, symbolTable);
}