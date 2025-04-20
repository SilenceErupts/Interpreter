/******************************************************************************
* @remark CS460: Programming Assignment 5 - Abstract Syntax Tree              *
 *                                                                            *
 * @author Shelby Anderson, Emilio Orozco, Jonathan Ramirez, Sam Tyler        *
 * @file  AST.cpp                                                             *
 * @date  April 22, 2025                                                      *
 *****************************************************************************/

#include "AST.hpp"
#include "ASTNode.hpp"
#include <iostream>

AST::AST(Node* CST, SymbolTable* symbolTable)
: _cst(CST), _symbolTable(symbolTable), _ast(nullptr), curScope(0), nextScopeId(1), braceDepth(0) {
    if (!_cst || !_symbolTable) {
        std::cerr << "AST: No CST or Symbol Table found\n";
        return;
    }
    buildAST();
}

void AST::buildAST() {
  Node* curCSTNode = _cst;
  ASTNode* tail = nullptr;

  while (curCSTNode != nullptr) {
    ASTNode* newASTNode = nullptr;
    const string &token = curCSTNode->name;

      if (token == "function" || token == "procedure") {
            //Create Declaration Statement
            newASTNode = createFuncProcDeclaration(curCSTNode);
            append(tail, newASTNode);
        }
      else if ( token == "int" || token == "char" || token == "bool" || token == "string") {
          vector<ASTNode*> variables = createVarDeclaration(curCSTNode);
          for (ASTNode* node : variables) {
              append(tail, node);
          }
      }
      else if (token == "{") {
          newASTNode = createBeginBlock(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "}") {
          newASTNode = createEndBlock(curCSTNode);
          append(tail, newASTNode);
      }
      else if (_symbolTable->lookup(token, curScope) || _symbolTable->lookup(token, 0)) {
          newASTNode = createAssignment(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "for") {
          newASTNode = createFor(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "while") {
          newASTNode = createWhile(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "if") {
          newASTNode = createIf(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "else") {
          newASTNode = createElse(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "return") {
          newASTNode = createReturn(curCSTNode);
          append(tail, newASTNode);
      }
      else if (token == "printf") {
          newASTNode = createPrintf(curCSTNode);
          append(tail, newASTNode);
      }
      else if (curCSTNode->rightSibling && curCSTNode->rightSibling->name == "(") {
          newASTNode = createCall(curCSTNode);
          append(tail, newASTNode);
      }
      else if (!token.empty()) {
          cerr << "Debug: Unhandled token: " << token << " on line " << curCSTNode->lineNumber << endl;
          exit(3);
          curCSTNode = grabNext(curCSTNode);
      }
  }
}

ASTNode* AST::lastRightSibling(ASTNode* node) {
    if (!node) {
        return nullptr;
    }
    while (node->rightSibling) {
        node = node->rightSibling;
        if (node->leftChild) {
            node = node->leftChild;
        }
    }
    return node;
}

void AST::append(ASTNode*& tail, ASTNode* newNode) {
    if (!newNode) {
        return;
    }
    if (!_ast) {
        _ast = newNode;
    } else {
        addChild(tail, newNode);
    }
    tail = lastRightSibling(newNode);
}

Node* AST::grabNext(Node* CST) {
    if (CST->rightSibling) {
        return CST->rightSibling;
    }
    if (CST->leftChild) {
        return CST->leftChild;
    }
    return nullptr;
}

ASTNode* AST::createFuncProcDeclaration(Node* &CST) {
    //cout << "DEBUG: CREATING FUNC/PROC DECLARATION for " << CST->name << endl;
  ASTNode* astDeclaration = new ASTNode {"DECLARATION", CST->lineNumber};
  string symbolTableNodeName;
  if (CST->name == "function") {
    symbolTableNodeName = CST->rightSibling->rightSibling->name;
    if(_symbolTable->lookup(symbolTableNodeName, nextScopeId)) {
      astDeclaration->symbol = _symbolTable->lookup(symbolTableNodeName, nextScopeId);
    }
    else {
      cerr << "Error in symbol table lookup\n";
    }
      while (grabNext(CST)->name != "{") {
          CST = grabNext(CST);
      }
  }
  if (CST->name == "procedure") {
      symbolTableNodeName = CST->rightSibling->name;
      if(_symbolTable->lookup(symbolTableNodeName, nextScopeId)) {
          astDeclaration->symbol = _symbolTable->lookup(symbolTableNodeName, nextScopeId);
      }
      else {
          cerr << "Error in symbol table lookup\n";
      }
      while (grabNext(CST)->name != "{") {
          CST = grabNext(CST);
      }
  }
    CST = grabNext(CST);
    return astDeclaration;
}

vector<ASTNode*> AST::createVarDeclaration(Node*& CST) {
    //cout << "DEBUG: CREATING var DECLARATION for " << CST->name << endl;
    vector<ASTNode*> astDeclaration;
    int declLine = CST->lineNumber;
    Node* cstNode = CST->rightSibling;
    while (cstNode && cstNode->name != ";") {
        if (cstNode->name == "," || cstNode->name == "[" || cstNode->name == "]") {
            // Skip delimiters and array brackets
            cstNode = cstNode->rightSibling;
            continue;
        }
        if (!isalpha(cstNode->name.at(0)) && cstNode->name.at(0) != '_') {
            cstNode = cstNode->rightSibling;
            continue;
        }
        string varName = cstNode->name;
        Symbol* sym = _symbolTable->lookup(varName, curScope);
        if (!sym) {
            cerr << "Error: variable `" << varName
                      << "` not found in scope " << curScope
                      << " at line " << cstNode->lineNumber << "\n";
        }
        ASTNode* decl = new ASTNode("DECLARATION", declLine);
        decl->symbol = sym;
        astDeclaration.push_back(decl);
        cstNode = cstNode->rightSibling;
    }
    CST = cstNode;
    CST = grabNext(CST);
    return astDeclaration;
}

ASTNode* AST::createBeginBlock(Node*& CST) {
    //cout << "DEBUG: CREATING begin block for " << CST->name << endl;
    braceDepth++;
    if (braceDepth == 1) {
        curScope = nextScopeId;
        nextScopeId++;
    }
    ASTNode* astBBlock = new ASTNode {"BEGIN BLOCK", CST->lineNumber};
    CST = grabNext(CST);
    return astBBlock;
}

ASTNode* AST::createEndBlock(Node*& CST) {
    //cout << "DEBUG: CREATING end block for " << CST->name << endl;
    braceDepth--;
    if (braceDepth == 0) {
        curScope = 0;
    }
    ASTNode* astEBlock = new ASTNode {"END BLOCK", CST->lineNumber};
    CST = grabNext(CST);
    return astEBlock;
}

ASTNode *AST::createPrintf(Node *&CST) {
    //cout << "DEBUG: CREATING printf for " << CST->name << endl;
    ASTNode* astPrintf = new ASTNode {"PRINTF", CST->lineNumber};
    Node* cstNode = CST->rightSibling;
    while (cstNode && cstNode->name != "(") cstNode = cstNode->rightSibling;
    if (cstNode) {
        cstNode = cstNode->rightSibling;
    }
    while (cstNode && cstNode->name != ")") {
        if (cstNode->name == "," || cstNode->name == "(" ||
            cstNode->name == "\"" || cstNode->name == "'") {
            cstNode = cstNode->rightSibling;
            continue;
        }
        ASTNode* param = new ASTNode{cstNode->name, cstNode->lineNumber};
        Symbol* symbol = _symbolTable->lookup(cstNode->name, curScope);
        if (symbol) {
            param->symbol = symbol;
        }
        addSibling(astPrintf, param);
        cstNode = cstNode->rightSibling;
    }
    while (CST && CST->name != ";") {
        CST = CST->rightSibling;
    }
    CST = grabNext(CST);
    return astPrintf;
}

ASTNode* AST::createReturn(Node*& CST) {
    //cout << "DEBUG: CREATING return for " << CST->name << endl;
    ASTNode* astReturn = new ASTNode("RETURN", CST->lineNumber);
    // Go past 'return'
    CST = CST->rightSibling;
    // move into expression
    if (CST && CST->name == "(") {
        CST = grabNext(CST);
    }
    ASTNode* expr = infixToPostfixBoolean(CST, true);
    if (expr) {
        addSibling(astReturn, expr);
    }
    if (CST && CST->name == ")") {
        CST = grabNext(CST);
    }
    if (CST && CST->name == ";") {
        CST = grabNext(CST);
    }
    return astReturn;
}

ASTNode* AST::createElse(Node*& CST) {
    //cout << "DEBUG: CREATING else for " << CST->name << endl;
    ASTNode* astElse = new ASTNode("ELSE", CST->lineNumber);
    CST = grabNext(CST);
    return astElse;
}

ASTNode* AST::createAssignment(Node*& CST) {
    int line = CST->lineNumber;
    ASTNode* astAssign = new ASTNode("ASSIGNMENT", line);

    // // check for array access
    if (CST->rightSibling && CST->rightSibling->name == "[") {
        ASTNode* base = new ASTNode(CST->name, line);
        base->symbol = _symbolTable->lookup(CST->name, curScope);
        addSibling(astAssign, base);

        CST = CST->rightSibling; // [
        ASTNode* lbrack = new ASTNode("[", CST->lineNumber);
        addSibling(astAssign, lbrack);

        CST = CST->rightSibling; // index
        ASTNode* index = new ASTNode(CST->name, CST->lineNumber);
        addSibling(astAssign, index);

        CST = CST->rightSibling; // ]
        ASTNode* rbrack = new ASTNode("]", CST->lineNumber);
        addSibling(astAssign, rbrack);

        CST = CST->rightSibling; // =
    } else {
        // normal variable
        std::string lhs = CST->name;
        Symbol* lhsSym = _symbolTable->lookup(lhs, curScope);
        if (!lhsSym) lhsSym = _symbolTable->lookup(lhs, 0);
        if (!lhsSym) {
            std::cerr << "Debug: Assignment error: variable '" << lhs
                      << "' not found in scope " << curScope
                      << " at line " << line << "\n";
            return nullptr;
        }
        ASTNode* lhsNode = new ASTNode(lhs, line);
        lhsNode->symbol = lhsSym;
        addSibling(astAssign, lhsNode);
        CST = CST->rightSibling;
    }

    // =
    if (!CST || CST->name != "=") {
        std::cerr << "Debug: Assignment error: expected '=' after LHS\n";
        return astAssign;
    }
    CST = CST->rightSibling;

    // rhs
    if (CST && CST->name == "'") {
        // Case: char literal
        ASTNode* quote1 = new ASTNode("'", CST->lineNumber);
        addSibling(astAssign, quote1);

        Node* inner = CST->rightSibling;
        if (inner) {
            ASTNode* charNode = new ASTNode(inner->name, inner->lineNumber);
            addSibling(astAssign, charNode);
            CST = inner;

            Node* closing = CST->rightSibling;
            if (closing && closing->name == "'") {
                ASTNode* quote2 = new ASTNode("'", closing->lineNumber);
                addSibling(astAssign, quote2);
                CST = closing->rightSibling;
            }
        }
    }
    else if (CST && CST->rightSibling && CST->rightSibling->name == "(") {
        // Case: Function Call
        ASTNode* func = new ASTNode(CST->name, CST->lineNumber);
        func->symbol = _symbolTable->lookup(CST->name, curScope);
        addSibling(astAssign, func);

        CST = CST->rightSibling; // now at "("
        ASTNode* lparen = new ASTNode("(", CST->lineNumber);
        addSibling(astAssign, lparen);
        CST = CST->rightSibling;

        // Arguments
        while (CST && CST->name != ")") {
            if (CST->name != ",") {
                ASTNode* arg = new ASTNode(CST->name, CST->lineNumber);
                if (auto sym = _symbolTable->lookup(CST->name, curScope))
                    arg->symbol = sym;
                addSibling(astAssign, arg);
            }
            CST = CST->rightSibling;
        }

        if (CST && CST->name == ")") {
            ASTNode* rparen = new ASTNode(")", CST->lineNumber);
            addSibling(astAssign, rparen);
            CST = grabNext(CST);
        }
    }
    else {
        ASTNode* rhs = infixToPostfixNumerical(CST, true);
        addSibling(astAssign, rhs);
    }
    ASTNode* assign = new ASTNode("=", line);
    addSibling(astAssign, assign);
    if (CST && CST->name == ";") {
        CST = grabNext(CST);
    }
    return astAssign;
}

ASTNode* AST::createIf(Node*& CST) {
    //cout << "DEBUG: CREATING if for " << CST->name << endl;
    ASTNode* astIf = new ASTNode("IF", CST->lineNumber);
    while (CST && CST->name != "(") {
        CST = grabNext(CST);
    }
    // Move past (
    if (CST && CST->name == "(") {
        CST = grabNext(CST);
    }
    ASTNode* condition = infixToPostfixBoolean(CST, false);
    if (condition) {
        addSibling(astIf, condition);
    }
    if (CST && CST->name == ")") {
        CST = grabNext(CST);
    }
    return astIf;
}

ASTNode* AST::createWhile(Node*& CST) {
    //cout << "DEBUG: CREATING while for " << CST->name << endl;
    ASTNode* astWhile = new ASTNode("WHILE", CST->lineNumber);
    while (CST && CST->name != "(") {
        CST = grabNext(CST);
    }
    // Move past (
    if (CST && CST->name == "(") {
        CST = grabNext(CST);
    }
    ASTNode* condition = infixToPostfixBoolean(CST, false);
    if (condition) {
        addSibling(astWhile, condition);
    }
    if (CST && CST->name == ")") {
        CST = grabNext(CST);
    }
    return astWhile;
}

ASTNode* AST::createFor(Node*& CST) {
    //cout << "DEBUG: CREATING for with token " << CST->name << endl;
    while (CST && CST->name != "(") {
        CST = grabNext(CST);
    }
    // move past (
    CST = grabNext(CST);
    // Expression 1 - assignment
    ASTNode* astFor = new ASTNode("FOR EXPRESSION 1", CST->lineNumber);
    ASTNode* assignExpr = createAssignment(CST);
    if (assignExpr && assignExpr->rightSibling) {
        // Skip ASSIGNMENT node
        ASTNode* tokens = assignExpr->rightSibling;
        addSibling(astFor, tokens);
    }
    // Expression 2 - bool
    ASTNode* expr2 = new ASTNode("FOR EXPRESSION 2", CST->lineNumber);
    ASTNode* condExpr = infixToPostfixBoolean(CST, true);
    addSibling(expr2, condExpr);
    addChild(astFor, expr2);
    // move past ;
    if (CST && CST->name == ";") {
        CST = grabNext(CST);
    }
    // Expression 3 - update
    ASTNode* expr3 = new ASTNode("FOR EXPRESSION 3", CST->lineNumber);
    ASTNode* updateExpr = infixToPostfixNumerical(CST, false);
    addSibling(expr3, updateExpr);
    addChild(expr2, expr3);
    // move past )
    if (CST && CST->name == ")") {
        CST = grabNext(CST);
    }
    return astFor;
}

ASTNode* AST::createCall(Node*& CST) {
    //std::cout << "DEBUG: CREATING call for " << CST->name << std::endl;
    ASTNode* astCall = new ASTNode("CALL", CST->lineNumber);
    ASTNode* funcName = new ASTNode(CST->name, CST->lineNumber);
    if (Symbol* symbol = _symbolTable->lookup(CST->name, curScope)) {
        funcName->symbol = symbol;
    }
    addSibling(astCall, funcName);
    //skip func name
    CST = CST->rightSibling;
    // Expect (
    if (CST && CST->name == "(") {
        ASTNode* lparen = new ASTNode("(", CST->lineNumber);
        addSibling(astCall, lparen);
        CST = CST->rightSibling;
    }
    // get args until )
    while (CST && CST->name != ")") {
        if (CST->name != ",") {
            ASTNode* arg = new ASTNode(CST->name, CST->lineNumber);
            if (Symbol* symbol = _symbolTable->lookup(CST->name, curScope))
                arg->symbol = symbol;
            addSibling(astCall, arg);
        }
        CST = CST->rightSibling;
    }
    if (CST && CST->name == ")") {
        ASTNode* rparen = new ASTNode(")", CST->lineNumber);
        addSibling(astCall, rparen);
        CST = CST->rightSibling;
    }
    // Skip over semicolon
    if (CST && CST->name == ";") {
        CST = grabNext(CST);
    }
    return astCall;
}

int AST::precedenceNumerical(const std::string &op) {
    if (op == "*" || op == "/" || op == "%") {
        return 3;
    }
    if (op == "+" || op == "-") {
        return 2;
    }
    if (op == "=") {
        return 1;
    }
    return 0;
}

bool AST::isNumericalOperator(const std::string &tok) {
    return tok == "+" || tok == "-" || tok == "*" ||
           tok == "/" || tok == "%" || tok == "=";
}

int AST::precedenceBoolean(const std::string &op) {
    if (op == "!") {
        return 3;
    }
    if (op == "&&" || op == "||") {
        return 1;
    }
    if (op == "==" || op == "!=" ||
        op == "<"  || op == "<=" ||
        op == ">"  || op == ">=" ) {
        return 2;
    }
    // fall back
    return precedenceNumerical(op);
}

bool AST::isBooleanOperator(const std::string &tok) {
    return tok == "!"  || tok == "&&" || tok == "||" ||
           tok == "==" || tok == "!=" || tok == "<" ||
           tok == "<=" || tok == ">"  || tok == ">=" || tok == "%";
}

ASTNode* AST::infixToPostfixNumerical(Node*& CST, bool stopOnSemi) {
    vector<ASTNode*> output;
    stack<ASTNode*> opstack;
    int parenDepth = 0;
    while (CST) {
        const string &tok = CST->name;
        int line = CST->lineNumber;
        if (!stopOnSemi && tok == ")" && parenDepth == 0) {
            break;  // closing paren for if and while
        }
        if (stopOnSemi && tok == ";") {
            break;  // end of assignment
        }
        if ((isdigit(tok.at(0)) ||
            (tok.at(0) == '-' && tok.length() > 1 && isdigit(tok.at(1)))) || //check for negative
            isalpha(tok.at(0)) || tok == "'" || tok == "\"") {
            output.push_back(new ASTNode{tok, line});
        }
        // consume (
        else if (tok == "(") {
            parenDepth++;
            opstack.push(new ASTNode{tok, line});
        }
        // consume )
        else if (tok == ")") {
            parenDepth--;
            while (!opstack.empty() && opstack.top()->name != "(") {
                output.push_back(opstack.top());
                opstack.pop();
            }
            if (!opstack.empty()) {
                delete opstack.top();
                opstack.pop();
            }
        }
        // operator
        else if (isNumericalOperator(tok)) {
            ASTNode* opNode = new ASTNode{tok, line};
            while (!opstack.empty()
                && isNumericalOperator(opstack.top()->name)
                && precedenceNumerical(opstack.top()->name)
                   >= precedenceNumerical(tok)) {
                output.push_back(opstack.top());
                opstack.pop();
            }
            opstack.push(opNode);
        }
        // get next token
        CST = grabNext(CST);
    }
    // empty stack after last token is read
    while (!opstack.empty()) {
        if (opstack.top()->name != "(") {
            output.push_back(opstack.top());
        }
        opstack.pop();
    }
    // link astnodes together
    ASTNode* head = nullptr;
    for (auto* n : output) {
        if (!head) {
            head = n;
        }
        else {
            addSibling(head, n);
        }
    }
    return head;
}

ASTNode* AST::infixToPostfixBoolean(Node*& CST, bool stopOnSemi) {
    vector<ASTNode*> output;
    stack<ASTNode*> opstack;
    int parenDepth = 0;
    while (CST) {
        const string& tok = CST->name;
        int line = CST->lineNumber;
        if (stopOnSemi && tok == ";") {
            break; //end of assignment
        }
        if (!stopOnSemi && tok == ")" && parenDepth == 0) {
            break; // closing paren for if and while
        }
        // char literal
        if (tok == "'") {
            output.push_back(new ASTNode{"'", line});
            Node* Char = CST->rightSibling;
            if (Char) {
                output.push_back(new ASTNode{Char->name, Char->lineNumber});
                CST = Char;
                Node* closingQuote = CST->rightSibling;
                if (closingQuote && closingQuote->name == "'") {
                    output.push_back(new ASTNode{"'", closingQuote->lineNumber});
                    CST = closingQuote;
                }
            }
        }
        // Case: Function call
        else if (isalpha(tok.at(0)) && CST->rightSibling && CST->rightSibling->name == "(") {
            output.push_back(new ASTNode{tok, line}); // func name
            output.push_back(new ASTNode{"(", CST->rightSibling->lineNumber}); // opening (
            CST = CST->rightSibling->rightSibling;

            while (CST && CST->name != ")") {
                if (CST->name != ",") {
                    output.push_back(new ASTNode{CST->name, CST->lineNumber});
                }
                CST = CST->rightSibling;
            }
            if (CST && CST->name == ")") {
                output.push_back(new ASTNode{")", CST->lineNumber});
            }
        }
        else if (isdigit(tok.at(0)) ||
                 (tok.at(0) == '-' && tok.length() > 1 && isdigit(tok.at(1))) || //check for negative
                 isalpha(tok.at(0)) ||
                 tok == "[" || tok == "]" || tok == "\"") {
            output.push_back(new ASTNode{tok, line});
        }
        // opening (
        else if (tok == "(") {
            parenDepth++;
            opstack.push(new ASTNode{tok, line});
        }
        // closing )
        else if (tok == ")") {
            parenDepth--;
            while (!opstack.empty() && opstack.top()->name != "(") {
                output.push_back(opstack.top());
                opstack.pop();
            }
            if (!opstack.empty()) {
                delete opstack.top();
                opstack.pop();
            }
        }
        // bool operator
        else if (isBooleanOperator(tok)) {
            ASTNode* opNode = new ASTNode{tok, line};
            while (!opstack.empty() && opstack.top()->name != "(" &&
                   precedenceBoolean(opstack.top()->name) >= precedenceBoolean(tok)) {
                output.push_back(opstack.top());
                opstack.pop();
            }
            opstack.push(opNode);
        }
        CST = grabNext(CST);
    }
    // empty stack after last token
    while (!opstack.empty()) {
        if (opstack.top()->name != "(") {
            output.push_back(opstack.top());
        }
        opstack.pop();
    }
    // link astnodes together
    ASTNode* head = nullptr;
    for (auto* n : output) {
        if (!head) {
            head = n;
        }
        else {
            addSibling(head, n);
        }
    }
    return head;
}

void AST::printASTWithSymbols(ASTNode* node) {
    if (!node) {
        return;
    }
    ASTNode* current = node;
    while (current) {
        // Print node name
        std::cout << current->name;
        // Print symbol if it exists
        if (current->symbol) {
            std::cout << " (" << current->symbol->name << ")";
        }
        // If right sibling, print arrow
        if (current->rightSibling) {
            std::cout << " -> ";
        }
        current = current->rightSibling;
    }
    std::cout << std::endl;
    // go down left child chain
    current = node;
    while (current) {
        if (current->leftChild) {
            printAST(current->leftChild);
        }
        current = current->rightSibling;
    }
}

void AST::printAST(ASTNode* node) {
    if (!node) {
        return;
    }
    ASTNode* current = node;
    while (current) {
        // Print node name
        std::cout << current->name;
        // If right sibling, print arrow
        if (current->rightSibling) {
            std::cout << " -> ";
        }
        current = current->rightSibling;
    }
    std::cout << std::endl;
    // go down left child chain
    current = node;
    while (current) {
        if (current->leftChild) {
            printAST(current->leftChild);
        }
        current = current->rightSibling;
    }
}
