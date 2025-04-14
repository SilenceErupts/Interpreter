
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <map>
#include <vector>

#include "Token.h"
#include "TokenList.h"
#include "Node.h"

using namespace std;

class Parser {
    public:
    Parser(TokenList &tokens);
    Node* parse();

    //Output Functions
    void graphicPrintTree(Node *root);
    Node *makeTerminalOnlyCST(Node *parseTreeRoot);
    //void writeTerminalsOnly(Node *root, ostream &out);

private:
    //Functions and Helpers for managing TokenList & Tree Node creation
    TokenList &m_tokens; // The TokenList we’re reading from
    Token currentToken(); // get the current token without removing it
    void advance(); // Advance to the next token
    void error(const std::string &msg); // throw error and exit
    Token peekNext();
    Node* createNodeFromToken(const Token &token);
    Node* match(Type expected);
    bool check(Type expected);
    bool checkAny(const vector<Type>& types);
    void attachNodes(Node* parent, const vector<Node*>& children);
    Node* buildNode(const string &label, const vector<Node*>& children);
    string tokenTypeToString(Type type);
    Token getLookahead(int n);

    // Each grammar rule as a function:
    Node *parseDATATYPE_SPECIFIER();
    Node *parseDOUBLE_QUOTED_STRING();
    Node *parseSINGLE_QUOTED_STRING();
    Node *parseIDENTIFIER();
    Node *parseIDENTIFIER_LIST();
    Node *parseIDENTIFIER_ARRAY_LIST();
    Node *parseIDENTIFIER_AND_IDENTIFIER_ARRAY_LIST();
    Node *parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST();
    Node *parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST_DECLARATION();
    Node *parseBOOLEAN_OPERATOR();
    Node *parseGETCHAR_FUNCTION();
    Node *parseSIZEOF_FUNCTION();
    Node *parseNUMERICAL_OPERATOR();
    Node *parseRELATIONAL_EXPRESSION();
    Node *parseNUMERICAL_EXPRESSION();
    Node *parseBOOLEAN_EXPRESSION();
    Node *parseEXPRESSION();
    Node *parseUSER_DEFINED_FUNCTION();
    Node *parseUSER_DEFINED_PROCEDURE_CALL_STATEMENT();
    Node *parseNUMERICAL_OPERAND();
    Node *parseINITIALIZATION_EXPRESSION();
    Node *parseITERATION_ASSIGNMENT();
    Node *parseASSIGNMENT_STATEMENT();
    Node *parseDECLARATION_STATEMENT();
    Node *parseRETURN_STATEMENT();
    Node *parseSELECTION_STATEMENT();
    Node *parsePRINTF_STATEMENT();
    Node *parseCOMPOUND_STATEMENT();
    Node *parseBLOCK_STATEMENT();
    Node *parseITERATION_STATEMENT();
    Node *parseSTATEMENT();
    Node *parsePARAMETER_LIST();
    Node *parseFUNCTION_DECLARATION();
    Node *parsePROCEDURE_DECLARATION();
    Node *parseMAIN_PROCEDURE();
    Node *parsePROGRAM_TAIL();
    Node* parsePROGRAM();
    //Grammar Helpers
    bool isBooleanExpression();
    bool startsStatement(Type t);
    //Output Helpers
    void printTreeHelper(Node *node, std::string prefix, bool isLastChild);
    void collectTerminalNodes(Node *node, vector<Node *> &terminals);
};


#endif //PARSER_H
