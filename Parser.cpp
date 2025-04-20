#include "Parser.h"
#include <queue>

Parser::Parser(TokenList &tokens)
    : m_tokens(tokens)
{}

Node* Parser::parse() {
    Node* root = parsePROGRAM();
    return root;
}

Token Parser::currentToken() {
    if (m_tokens.empty()) {
        std::cerr << "[ERROR] Token list empty when calling currentToken()\n";
        return Token(Type::END_OF_FILE, "", -1);
    }
    return m_tokens.front();
}

void Parser::advance() {
    if (!m_tokens.empty()) {
        m_tokens.pop_front();
    }
}

void Parser::error(const std::string &msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

Token Parser::peekNext() {
    return m_tokens.peekNext();
}

Node* Parser::createNodeFromToken(const Token &token) {
    //cout << "[DEBUG] Creating node: " << token.getText() << " at line " << token.getLine() << std::endl;
    return new Node(token.getText(), token.getLine());
}

Node* Parser::match(Type expected) {
    if (currentToken().getType() != expected) {
        error("Expected token of type " + tokenTypeToString(expected) +
              ", but got " + currentToken().getTypeName() +
              " on line " + std::to_string(currentToken().getLine()));
    }
    Node* node = createNodeFromToken(currentToken());
    advance();
    return node;
}

bool Parser::check(Type expected) {
    return currentToken().getType() == expected;
}

bool Parser::checkAny(const vector<Type> &types) {
    for (Type t : types) {
        if (currentToken().getType() == t) {
            return true;
        }
    }
    return false;
}

void Parser::attachNodes(Node *parent, const vector<Node *> &children) {
    //std::cout << "[DEBUG] Attaching children to: " << parent->name << std::endl;
    if (children.empty()) {
        return;
    }
    parent->leftChild = children[0];
    Node* current = children[0];
    for (int i = 1; i < children.size(); i++) {
        if (current == children[i]) {
            std::cerr << "[ERROR] Circular reference while attaching children to "
                      << parent->name << "\n";
            exit(1);
        }
        current->rightSibling = children[i];
        current = current->rightSibling;
    }
}

Node* Parser::buildNode(const string &label, const vector<Node *> &children) {
    int line = children.empty() ? currentToken().getLine() : children[0]->lineNumber;
    //std::cout << "[DEBUG] buildNode: " << label << " with " << children.size() << " children\n";
    // Detect if any child is null
    for (Node* child : children) {
        if (!child) {
            std::cerr << "[ERROR] Null child passed to buildNode for: " << label << "\n";
            exit(1);
        }
    }
    // Detect if same pointer appears twice
    for (int i = 0; i < children.size(); ++i) {
        for (int j = i + 1; j < children.size(); ++j) {
            if (children[i] == children[j]) {
                std::cerr << "[ERROR] Duplicate child pointer detected in buildNode: " << label << "\n";
                exit(1);
            }
        }
    }
    Node* node = new Node(label, line);
    //std::cout << "[DEBUG] Created node [" << label << "] with ID: " << node->id << "\n";
    attachNodes(node, children);
    return node;
}

string Parser::tokenTypeToString(Type type) {
    switch (type) {
        case Type::IDENTIFIER:         return "IDENTIFIER";
        case Type::INTEGER:            return "INTEGER";
        case Type::STRING:             return "STRING";
        case Type::DOUBLE_QUOTE:       return "DOUBLE_QUOTE";
        case Type::SINGLE_QUOTE:       return "SINGLE_QUOTE";
        case Type::RETURN_KEYWORD:     return "IDENTIFIER";
        case Type::BOOLEAN_TRUE:       return "IDENTIFIER";
        case Type::BOOLEAN_FALSE:      return "IDENTIFIER";
        case Type::BOOLEAN_NOT:        return "BOOLEAN_NOT";
        case Type::BOOLEAN_EQUAL:      return "BOOLEAN_EQUAL";
        case Type::BOOLEAN_NOT_EQUAL:  return "BOOLEAN_NOT_EQUAL";
        case Type::BOOLEAN_AND:        return "BOOLEAN_AND";
        case Type::BOOLEAN_OR:         return "BOOLEAN_OR";
        case Type::ASSIGNMENT_OPERATOR:return "ASSIGNMENT_OPERATOR";
        case Type::PLUS:               return "PLUS";
        case Type::MINUS:              return "MINUS";
        case Type::ASTERISK:           return "ASTERISK";
        case Type::DIVIDE:             return "DIVIDE";
        case Type::MODULO:             return "MODULO";
        case Type::LT_EQUAL:           return "LT_EQUAL";
        case Type::GT_EQUAL:           return "GT_EQUAL";
        case Type::LT:                 return "LT";
        case Type::GT:                 return "GT";
        case Type::SEMICOLON:          return "SEMICOLON";
        case Type::COMMA:              return "COMMA";
        case Type::INT:                return "INT";
        case Type::BOOL:               return "BOOL";
        case Type::CHAR:               return "CHAR";
        case Type::GETCHAR:            return "GETCHAR";
        case Type::VOID:                return "VOID";
        case Type::IF:                 return "IF";
        case Type::ELSE:                return "ELSE";
        case Type::WHILE:              return "WHILE";
        case Type::FOR:                return "FOR";
        case Type::PRINTF:             return "PRINTF";
        case Type::FUNCTION:           return "FUNCTION";
        case Type::CARET:               return "CARET";
        case Type::SIZEOF:              return "SIZEOF";
        case Type::PROCEDURE:           return "PROCEDURE";
        case Type::L_PAREN:            return "L_PAREN";
        case Type::R_PAREN:            return "R_PAREN";
        case Type::L_BRACE:            return "L_BRACE";
        case Type::R_BRACE:            return "R_BRACE";
        case Type::L_BRACKET:          return "L_BRACKET";
        case Type::R_BRACKET:          return "R_BRACKET";
        case Type::DOUBLE_QUOTED_STRING:return "DOUBLE_QUOTED_STRING";
        case Type::SINGLE_QUOTED_STRING:return "SINGLE_QUOTED_STRING";
        case Type::END_OF_FILE:        return "END_OF_FILE";
        // Error states
        case Type::ERROR_INVALID_INT:       return "ERROR_INVALID_INT";
        case Type::ERROR_INVALID_CHAR:      return "ERROR_INVALID_CHAR";
        case Type::ERROR_INVALID_OPERATOR:  return "ERROR_INVALID_OPERATOR";
        case Type::ERROR_UNCLOSED_STRING:   return "ERROR_UNCLOSED_STRING";
        case Type::ERROR_UNCLOSED_CHAR:     return "ERROR_UNCLOSED_CHAR";
        case Type::ERROR_INVALID_IDENTIFIER:return "ERROR_INVALID_IDENTIFIER";
        default:
            return "UNKNOWN_TOKEN_TYPE";
    }
}

Token Parser::getLookahead(int n) {
    TokenNode *cur = m_tokens.getHead();
    while(n > 0 && cur != nullptr) {
        cur = cur->next; n--;
    }
    if (cur != nullptr) {
        return cur->token;
    }
    return Token(Type::END_OF_FILE, "", -1);
}

// <DATATYPE_SPECIFIER> ::= char | bool | int
Node* Parser::parseDATATYPE_SPECIFIER() {
    vector<Type> validTypes = { Type::CHAR, Type::BOOL, Type::INT };
    if (!checkAny(validTypes)) {
        error("Expected datatype specifier (char, bool, int), but got: " +
              currentToken().getText() + " on line " +
              std::to_string(currentToken().getLine()));
    }
    return match(currentToken().getType());
}

//<DOUBLE_QUOTED_STRING> ::= <DOUBLE_QUOTE> <STRING> <DOUBLE_QUOTE>
Node* Parser::parseDOUBLE_QUOTED_STRING() {
    Node* leftQuote = match(Type::DOUBLE_QUOTE);
    if (currentToken().getType() != Type::STRING) {
        error("Expected string literal after opening double quote on line " +
              std::to_string(currentToken().getLine()));
    }
    Node* content = match(Type::STRING);
    Node* rightQuote = match(Type::DOUBLE_QUOTE);
    return buildNode("DoubleQuotedString", { leftQuote, content, rightQuote });
}

//<SINGLE_QUOTED_STRING> ::= <SINGLE_QUOTE> <STRING> <SINGLE_QUOTE>
Node* Parser::parseSINGLE_QUOTED_STRING() {
    Node* leftQuote = match(Type::SINGLE_QUOTE);
    if (currentToken().getType() != Type::STRING) {
        error("Expected string literal after opening single quote on line " +
              std::to_string(currentToken().getLine()));
    }
    Node* content = match(Type::STRING);
    Node* rightQuote = match(Type::SINGLE_QUOTE);
    return buildNode("SingleQuotedString", { leftQuote, content, rightQuote });
}

Node* Parser::parseIDENTIFIER() {
    Token tok = currentToken();
    std::string text = tok.getText();
    Type type = tok.getType();
    // Check for reserved word
    if (text == "char" || text == "bool" || text == "int" ||
        text == "return" || text == "void" || text == "printf" ||
        text == "getchar" || text == "sizeof" || text == "function" || text == "procedure" ||
        text == "if" || text == "else" || text == "while" || text == "for") {
            std::string role = "variable";
            if (peekNext().getType() == Type::L_PAREN) {
                role = "function";
            }
        error("Syntax error on line " + std::to_string(tok.getLine()) +
              ": reserved word \"" + text + "\" cannot be used for the name of a " + role + ".");
        }
    if (type != Type::IDENTIFIER) {
        error("Expected identifier, but got \"" + text + "\" on line " +
              std::to_string(tok.getLine()));
    }
    return match(Type::IDENTIFIER);
}

// <IDENTIFIER_LIST> ::= <IDENTIFIER> | <IDENTIFIER> <COMMA> <IDENTIFIER_LIST>
Node* Parser::parseIDENTIFIER_LIST() {
    std::vector<Node*> children;
    // <IDENTIFIER>
    children.push_back(parseIDENTIFIER());
    // if <COMMA>, expect <IDENTIFIER_LIST>
    while (check(Type::COMMA)) {
        Node* comma = match(Type::COMMA);
        Node* id = parseIDENTIFIER();
        children.push_back(comma);
        children.push_back(id);
    }
    return buildNode("IdentifierList", children);
}

Node* Parser::parseIDENTIFIER_ARRAY_LIST() {
    std::vector<Node*> children;
    // <IDENTIFIER> <L_BRACKET> <WHOLE_NUMBER> <R_BRACKET>
    children.push_back(parseIDENTIFIER());
    children.push_back(match(Type::L_BRACKET));
    if (currentToken().getType() != Type::INTEGER) {
        error("Expected whole number inside array brackets on line " +
              to_string(currentToken().getLine()));
    }
    // make sure integer is positive
    Token numberToken = currentToken();
    int value = stoi(numberToken.getText());
    if (value <= 0) {
        error("Syntax error on line " + std::to_string(numberToken.getLine()) +
              ": array declaration size must be a positive integer.");
    }
    children.push_back(match(Type::INTEGER));
    children.push_back(match(Type::R_BRACKET));
    // if <COMMA>, then <IDENTIFIER_ARRAY_LIST>
    if (check(Type::COMMA)) {
        children.push_back(match(Type::COMMA));
        Node* rest = parseIDENTIFIER_ARRAY_LIST();
        children.push_back(rest);
    }
    return buildNode("IdentifierArrayList", children);
}

/* <IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST> ::= <IDENTIFIER_LIST> |
 * <IDENTIFIER_ARRAY_LIST> |
 * <IDENTIFIER_LIST> <IDENTIFIER_ARRAY_LIST> |
 * <IDENTIFIER_ARRAY_LIST> <IDENTIFIER_LIST>
*/
Node* Parser::parseIDENTIFIER_AND_IDENTIFIER_ARRAY_LIST() {
    std::vector<Node*> children;
    // <IDENTIFIER_LIST> | <IDENTIFIER_ARRAY_LIST>
    if (check(Type::IDENTIFIER)) {
        // Peek to determine if this is part of an array or just an identifier
        Token second = peekNext();
        if (second.getType() == Type::L_BRACKET) {
            // <IDENTIFIER_ARRAY_LIST>
            children.push_back(parseIDENTIFIER_ARRAY_LIST());
            // if <COMMA>, then <IDENTIFIER_LIST>
            if (check(Type::COMMA)) {
                children.push_back(match(Type::COMMA));
                children.push_back(parseIDENTIFIER_LIST());
            }
        } else {
            // <IDENTIFIER_LIST>
            children.push_back(parseIDENTIFIER_LIST());
            // if <COMMA>, then <IDENTIFIER_ARRAY_LIST>
            if (check(Type::COMMA)) {
                children.push_back(match(Type::COMMA));
                children.push_back(parseIDENTIFIER_ARRAY_LIST());
            }
        }
    }
    else {
        error("Expected identifier or identifier array declaration at line " +
              std::to_string(currentToken().getLine()));
    }
    return buildNode("IdentifierAndIdentifierArrayList", children);
}

// <IDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST> ::= ...
Node* Parser::parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST() {
    std::vector<Node*> children;
    // <IDENTIFIER>
    Node* id = parseIDENTIFIER();
    children.push_back(id);
    // Check for array form
    if (check(Type::L_BRACKET)) {
        children.push_back(match(Type::L_BRACKET));
        if (check(Type::IDENTIFIER)) {
            children.push_back(parseIDENTIFIER());
        } else {
            children.push_back(parseNUMERICAL_EXPRESSION());
        }
        children.push_back(match(Type::R_BRACKET));
    }
    // if <COMMA>, then <IDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST>
    if (check(Type::COMMA)) {
        children.push_back(match(Type::COMMA));
        Node* rest = parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST();
        children.push_back(rest);
    }
    return buildNode("IdentifierAndIdentifierArrayParameterList", children);
}

Node* Parser::parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST_DECLARATION() {
    std::vector<Node*> children;
    // <IDENTIFIER>
    Node* id = parseIDENTIFIER();
    children.push_back(id);
    // if <L_BRACKET>, then WHOLE_NUMBER <R_BRACKET>
    if (check(Type::L_BRACKET)) {
        children.push_back(match(Type::L_BRACKET));
        if (currentToken().getType() != Type::INTEGER) {
            error("Expected whole number in array declaration on line " +
                  std::to_string(currentToken().getLine()));
        }
        children.push_back(match(Type::INTEGER));
        children.push_back(match(Type::R_BRACKET));
    }
    // if <COMMA>, then <IDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST_DECLARATION>
    if (check(Type::COMMA)) {
        children.push_back(match(Type::COMMA));
        //Node* rest = parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST_DECLARATION();
        Node* rest = parsePARAMETER_LIST();
        children.push_back(rest);
    }
    return buildNode("IdentifierAndIdentifierArrayParameterListDeclaration", children);
}

// <BOOLEAN_OPERATOR> ::= <BOOLEAN_AND> | <BOOLEAN_OR>
Node* Parser::parseBOOLEAN_OPERATOR() {
    if (!checkAny({ Type::BOOLEAN_AND, Type::BOOLEAN_OR })) {
        error("Expected boolean operator (&& or ||), but got '" +
              currentToken().getText() + "' on line " +
              std::to_string(currentToken().getLine()));
    }
    return match(currentToken().getType());
}

// <GETCHAR_FUNCTION> ::= getchar <L_PAREN> <IDENTIFIER> <R_PAREN>
Node* Parser::parseGETCHAR_FUNCTION() {
    std::vector<Node*> children;
    children.push_back(match(Type::GETCHAR));
    children.push_back(match(Type::L_PAREN));
    if (currentToken().getType() != Type::VOID) {
        error("Expected 'void' in getchar function call on line " +
              std::to_string(currentToken().getLine()));
    }
    children.push_back(match(Type::VOID));
    children.push_back(match(Type::R_PAREN));
    return buildNode("GetcharFunction", children);
}

Node* Parser::parseSIZEOF_FUNCTION() {
    std::vector<Node*> children;
    children.push_back(match(Type::SIZEOF));
    children.push_back(match(Type::L_PAREN));
    children.push_back(parseIDENTIFIER());
    children.push_back(match(Type::R_PAREN));
    return buildNode("SizeofFunction", children);
}

// <NUMERICAL_OPERATOR> ::= <PLUS> | <MINUS> | <ASTERISK> | <DIVIDE> | <MODULO> | <CARET>
Node* Parser::parseNUMERICAL_OPERATOR() {
    vector<Type> validOperators = {
        Type::PLUS,
        Type::MINUS,
        Type::ASTERISK,
        Type::DIVIDE,
        Type::MODULO,
        Type::CARET
    };
    if (!checkAny(validOperators)) {
        error("Expected numerical operator (+, -, *, /, %, ^) but got '" +
              currentToken().getText() + "' on line " +
              std::to_string(currentToken().getLine()));
    }
    return match(currentToken().getType());
}

Node* Parser::parseRELATIONAL_EXPRESSION() {
    std::vector<Type> relationalTypes = {
        Type::LT,
        Type::LT_EQUAL,
        Type::GT,
        Type::GT_EQUAL,
        Type::BOOLEAN_EQUAL,
        Type::BOOLEAN_NOT_EQUAL
    };
    if (!checkAny(relationalTypes)) {
        error("Expected relational operator (<, <=, >, >=, ==, !=) but got '" +
              currentToken().getText() + "' on line " +
              std::to_string(currentToken().getLine()));
    }
    return match(currentToken().getType());
}

Node* Parser::parseNUMERICAL_EXPRESSION() {
    if (checkAny({ Type::PLUS, Type::MINUS, Type::ASTERISK, Type::DIVIDE, Type::MODULO, Type::CARET })) {
        Node* unaryOp = parseNUMERICAL_OPERATOR();
        //cout << "[DEBUG] Consumed NUMERICAL_OPERATOR (unary)" << endl;
        Node* expr = parseNUMERICAL_EXPRESSION();
        return buildNode("NUMERICAL_EXPRESSION", { unaryOp, expr });
    }
    if (check(Type::L_PAREN)) {
        Node* lparen = match(Type::L_PAREN);
        //cout << "[DEBUG] Consumed L_PAREN" << endl;
        Node* operand = parseNUMERICAL_OPERAND();
        if (currentToken().getType() == Type::BOOLEAN_EQUAL) {
            Node* eqNode = match(Type::BOOLEAN_EQUAL);
            //cout << "[DEBUG] Consumed BOOLEAN_EQUAL" << endl;
            if (currentToken().getType() == Type::INTEGER) {
                Node* intNode = match(Type::INTEGER);
                //cout << "[DEBUG] Consumed INTEGER" << endl;
                if (currentToken().getType() == Type::R_PAREN) {
                    Node* rparen = match(Type::R_PAREN);
                    //cout << "[DEBUG] Consumed R_PAREN" << endl;
                    return buildNode("NUMERICAL_EXPRESSION", { lparen, operand, eqNode, intNode, rparen });
                }
            }
        }
        if (check(Type::R_PAREN)) {
            Node* rparen = match(Type::R_PAREN);
            return buildNode("NUMERICAL_EXPRESSION", { lparen, operand, rparen });
        }
        if (checkAny({ Type::PLUS, Type::MINUS, Type::ASTERISK, Type::DIVIDE, Type::MODULO, Type::CARET })) {
            Node* op = parseNUMERICAL_OPERATOR();
            //cout << "[DEBUG] Consumed NUMERICAL_OPERATOR" << endl;
            //cout << "[DEBUG] Moving to numerical expression" << endl;
            Node* innerExpr = parseNUMERICAL_EXPRESSION();
            if (check(Type::R_PAREN)) {
                Node* rparen = match(Type::R_PAREN);
                //cout << "[DEBUG] Consumed R_PAREN" << endl;
                if (checkAny({ Type::PLUS, Type::MINUS, Type::ASTERISK, Type::DIVIDE, Type::MODULO, Type::CARET })) {
                    Node* op2 = parseNUMERICAL_OPERATOR();
                    //cout << "[DEBUG] Consumed NUMERICAL_OPERATOR" << endl;
                    Node* expr2 = parseNUMERICAL_EXPRESSION();
                    return buildNode("NUMERICAL_EXPRESSION", { lparen, operand, op, innerExpr, rparen, op2, expr2 });
                }
                return buildNode("NUMERICAL_EXPRESSION", { lparen, operand, op, innerExpr, rparen });
            }
        }
    }
    Node* operand = parseNUMERICAL_OPERAND();
    if (!checkAny({ Type::PLUS, Type::MINUS, Type::ASTERISK, Type::DIVIDE, Type::MODULO, Type::CARET })) {
        return buildNode("NUMERICAL_EXPRESSION", { operand });
    }
    Node* op = parseNUMERICAL_OPERATOR();
    if (check(Type::L_PAREN)) {
        Node* lparen = match(Type::L_PAREN);
        Node* innerExpr = parseNUMERICAL_EXPRESSION();
        if (!check(Type::R_PAREN)) {
            error("Syntax error on line " + std::to_string(currentToken().getLine()) +
                  ": expected ')' after parenthesized expression.");
        }
        Node* rparen = match(Type::R_PAREN);
        Node* parenExpr = buildNode("NUMERICAL_EXPRESSION", { lparen, innerExpr, rparen });
        if (checkAny({ Type::PLUS, Type::MINUS, Type::ASTERISK, Type::DIVIDE, Type::MODULO, Type::CARET })) {
            Node* op2 = parseNUMERICAL_OPERATOR();
            Node* expr2 = parseNUMERICAL_EXPRESSION();
            return buildNode("NUMERICAL_EXPRESSION", { operand, op, parenExpr, op2, expr2 });
        }
        return buildNode("NUMERICAL_EXPRESSION", { operand, op, parenExpr });
    } else {
        Node* restExpr = parseNUMERICAL_EXPRESSION();
        return buildNode("NUMERICAL_EXPRESSION", { operand, op, restExpr });
    }
}

Node* Parser::parseBOOLEAN_EXPRESSION() {
    if ((getLookahead(1).getType() == Type::BOOLEAN_EQUAL ||
        getLookahead(1).getType() == Type::LT_EQUAL ||
        getLookahead(1).getType() == Type::GT_EQUAL ||
        getLookahead(1).getType() == Type::LT ||
        getLookahead(1).getType() == Type::GT) ||
        (currentToken().getType() == Type::L_PAREN && getLookahead(1).getType() == Type::IDENTIFIER && (getLookahead(2).getType() == Type::PLUS ||
            getLookahead(2).getType() == Type::MINUS || getLookahead(2).getType() == Type::ASTERISK || getLookahead(2).getType() == Type::DIVIDE ||
            getLookahead(2).getType() == Type::MODULO))) {
        Node* leftNumExpr = parseNUMERICAL_EXPRESSION(); Node* relOp = match(currentToken().getType());
        //cout << "[DEBUG] Consumed RELATIONAL_OPERATOR" << endl;
        Node* rightNumExpr = parseNUMERICAL_EXPRESSION();
        return buildNode("BOOLEAN_EXPRESSION", { leftNumExpr, relOp, rightNumExpr });
    }
    if (currentToken().getType() == Type::BOOLEAN_TRUE) {
        Node* trueNode = match(Type::BOOLEAN_TRUE);
        //cout << "[DEBUG] Consumed BOOLEAN_TRUE" << endl;
        return buildNode("BOOLEAN_EXPRESSION", { trueNode });
    }
    if (currentToken().getType() == Type::BOOLEAN_FALSE) {
        Node* falseNode = match(Type::BOOLEAN_FALSE);
        //cout << "[DEBUG] Consumed BOOLEAN_FALSE" << endl;
        return buildNode("BOOLEAN_EXPRESSION", { falseNode });
    }
    if (currentToken().getType() == Type::L_PAREN &&
        getLookahead(1).getType() == Type::IDENTIFIER &&
        (getLookahead(2).getType() == Type::BOOLEAN_AND ||
         getLookahead(2).getType() == Type::BOOLEAN_OR)) {
        Node* lparen = match(Type::L_PAREN);
        //cout << "[DEBUG] Consumed L_PAREN" << endl;
        Node* idNode = parseIDENTIFIER();
        Node* boolOp = match(currentToken().getType());
        Node* nextNode = match(currentToken().getType());
        Node* innerExpr = parseBOOLEAN_EXPRESSION();
        if (currentToken().getType() == Type::R_PAREN) {
            Node* rparen = match(Type::R_PAREN);
            //cout << "[DEBUG] Consumed R_PAREN" << endl;
            return buildNode("BOOLEAN_EXPRESSION", { lparen, idNode, boolOp, nextNode, innerExpr, rparen });
        }
    }
    if (currentToken().getType() == Type::L_PAREN) {
        Node* lparen = match(Type::L_PAREN);
        //cout << "[DEBUG] Consumed L_PAREN" << endl;
        if (currentToken().getType() == Type::BOOLEAN_NOT) {
            Node* notNode = match(Type::BOOLEAN_NOT);
            Node* operand = parseNUMERICAL_OPERAND();
            Node* rparen = match(Type::R_PAREN);
            return buildNode("BOOLEAN_EXPRESSION", { lparen, notNode, operand, rparen });
        } else {
            Node* operand = parseNUMERICAL_OPERAND();
            if (checkAny({Type::BOOLEAN_EQUAL, Type::BOOLEAN_NOT_EQUAL,
                          Type::LT_EQUAL, Type::GT_EQUAL, Type::LT, Type::GT})) {
                Node* relOp = parseRELATIONAL_EXPRESSION();
                Node* operand2 = parseNUMERICAL_OPERAND();
                Node* rparen = match(Type::R_PAREN);
                vector<Node*> group = { lparen, operand, relOp, operand2, rparen };
                if (checkAny({Type::BOOLEAN_AND, Type::BOOLEAN_OR})) {
                    Node* boolOp = parseBOOLEAN_OPERATOR();
                    Node* rest = parseBOOLEAN_EXPRESSION();
                    group.push_back(boolOp);
                    group.push_back(rest);
                }
                return buildNode("BOOLEAN_EXPRESSION", group);
            }
        }
    }
    if (currentToken().getType() == Type::IDENTIFIER && peekNext().getType() == Type::L_BRACKET) {
        Node* idArr = parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST();
        if (currentToken().getType() == Type::BOOLEAN_EQUAL) {
            Node* eqNode = match(Type::BOOLEAN_EQUAL);
            //cout << "[DEBUG] Consumed BOOLEAN_OPERATOR" << endl;
            //cout << "[DEBUG] Moving to numerical expression" << endl;
            Node* numExpr = parseNUMERICAL_EXPRESSION();
            return buildNode("BOOLEAN_EXPRESSION", { idArr, eqNode, numExpr });
        }
    }
    if (currentToken().getType() == Type::BOOLEAN_NOT) {
        Node* notNode = match(Type::BOOLEAN_NOT);
        Node* numExpr = parseNUMERICAL_EXPRESSION();
        return buildNode("BOOLEAN_EXPRESSION", { notNode, numExpr });
    }
    if (currentToken().isIdentifier()) {
        Node* id = parseIDENTIFIER();
        if (checkAny({Type::BOOLEAN_AND, Type::BOOLEAN_OR})) {
            Node* boolOp = parseBOOLEAN_OPERATOR();
            Node* rest = parseBOOLEAN_EXPRESSION();
            return buildNode("BOOLEAN_EXPRESSION", { id, boolOp, rest });
        }
        return buildNode("BOOLEAN_EXPRESSION", { id });
    }
    error("Syntax error on line " + std::to_string(currentToken().getLine()) +
          ": could not parse boolean expression.");
    return nullptr;
}

bool Parser::isBooleanExpression() {
    TokenNode* cur = m_tokens.getHead();
    int parenDepth = 0;
    while (cur != nullptr) {
        Type type = cur->token.getType();
        // break on delimiter if not in parenthesis
        if (parenDepth == 0 && (type == Type::SEMICOLON || type == Type::COMMA ||
            type == Type::R_PAREN || type == Type::R_BRACKET || type == Type::R_BRACE)) {
            break;
            }
        if (type == Type::L_PAREN) {
            parenDepth++;
        } else if (type == Type::R_PAREN) {
            if (parenDepth > 0)
                parenDepth--;
            else
                break;
        }
        // Check for boolean operators or relational
        if (type == Type::BOOLEAN_AND || type == Type::BOOLEAN_OR || type == Type::BOOLEAN_NOT ||
            type == Type::BOOLEAN_TRUE || type == Type::BOOLEAN_FALSE ||
            type == Type::BOOLEAN_EQUAL || type == Type::BOOLEAN_NOT_EQUAL ||
            type == Type::LT || type == Type::GT || type == Type::LT_EQUAL || type == Type::GT_EQUAL) {
            return true;
            }
        cur = cur->next;
    }
    return false;
}

Node* Parser::parseEXPRESSION() {
    Node* child = nullptr;
    if (isBooleanExpression()) {
        child = parseBOOLEAN_EXPRESSION();
    } else {
        child = parseNUMERICAL_EXPRESSION();
    }
    return buildNode("EXPRESSION", { child });
}

Node* Parser::parseUSER_DEFINED_FUNCTION() {
    std::vector<Node*> children;
    Node* id = parseIDENTIFIER();
    children.push_back(id);
    children.push_back(match(Type::L_PAREN));
    if (isBooleanExpression() || check(Type::INTEGER) ||
        check(Type::L_PAREN) || check(Type::SINGLE_QUOTE) ||
        check(Type::DOUBLE_QUOTE) || check(Type::GETCHAR) ||
        check(Type::SIZEOF) || check(Type::PLUS) || check(Type::MINUS)) {
        // <EXPRESSION>
        Node* expr = parseEXPRESSION();
        children.push_back(expr);
    } else {
        // <IDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST>
        Node* params = parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST();
        children.push_back(params);
    }
    children.push_back(match(Type::R_PAREN));
    return buildNode("UserDefinedFunction", children);
}

Node* Parser::parseUSER_DEFINED_PROCEDURE_CALL_STATEMENT() {
    std::vector<Node*> children;
    Node* id = parseIDENTIFIER();
    children.push_back(id);
    children.push_back(match(Type::L_PAREN));
    if (isBooleanExpression() || check(Type::INTEGER) || check(Type::L_PAREN) ||
        check(Type::SINGLE_QUOTE) || check(Type::DOUBLE_QUOTE) || check(Type::GETCHAR) ||
        check(Type::SIZEOF) || check(Type::PLUS) || check(Type::MINUS)) {
        Node* expr = parseEXPRESSION();
        children.push_back(expr);
        } else {
            Node* params = parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST();
            children.push_back(params);
        }
    children.push_back(match(Type::R_PAREN));
    children.push_back(match(Type::SEMICOLON));
    return buildNode("UserDefinedProcedureCallStatement", children);
}

Node* Parser::parseNUMERICAL_OPERAND() {
    std::vector<Node*> children;
    // <INTEGER>
    if (check(Type::INTEGER)) {
        children.push_back(match(Type::INTEGER));
        return buildNode("NumericalOperand", children);
    }
    if (check(Type::IDENTIFIER)) {
        Token next = peekNext();
        // <IDENTIFIER> <L_BRACKET> <NUMERICAL_EXPRESSION> <R_BRACKET>
        if (next.getType() == Type::L_BRACKET) {
            children.push_back(parseIDENTIFIER());
            children.push_back(match(Type::L_BRACKET));
            children.push_back(parseNUMERICAL_EXPRESSION());
            children.push_back(match(Type::R_BRACKET));
            return buildNode("NumericalOperand", children);
        }
        if (next.getType() == Type::L_PAREN) {
            Node* udf = parseUSER_DEFINED_FUNCTION();
            return buildNode("NumericalOperand", { udf });
        }
        // Just an IDENTIFIER
        children.push_back(parseIDENTIFIER());
        return buildNode("NumericalOperand", children);
    }
    // GETCHAR_FUNCTION
    if (check(Type::GETCHAR)) {
        Node* g = parseGETCHAR_FUNCTION();
        return buildNode("NumericalOperand", { g });
    }
    // SIZEOF_FUNCTION
    if (check(Type::SIZEOF)) {
        Node* s = parseSIZEOF_FUNCTION();
        return buildNode("NumericalOperand", { s });
    }
    // SINGLE_QUOTED_STRING
    if (check(Type::SINGLE_QUOTE)) {
        Node* sq = parseSINGLE_QUOTED_STRING();
        return buildNode("NumericalOperand", { sq });
    }
    // DOUBLE_QUOTED_STRING
    if (check(Type::DOUBLE_QUOTE)) {
        Node* dq = parseDOUBLE_QUOTED_STRING();
        return buildNode("NumericalOperand", { dq });
    }
    error("Invalid numerical operand at line " + to_string(currentToken().getLine()));
    return nullptr;
}

Node* Parser::parseINITIALIZATION_EXPRESSION() {
    std::vector<Node*> children;
    //std::cout << "[DEBUG] Entering parseINITIALIZATION_EXPRESSION()\n";
    Node* id = parseIDENTIFIER();
    children.push_back(id);
    //std::cout << "[DEBUG] Got identifier: " << id->name << "\n";
    Node* assign = match(Type::ASSIGNMENT_OPERATOR);
    children.push_back(assign);
    //std::cout << "[DEBUG] Matched assignment operator\n";
    if (check(Type::SINGLE_QUOTE)) {
        //std::cout << "[DEBUG] Parsing single quoted string\n";
        children.push_back(parseSINGLE_QUOTED_STRING());
    } else if (check(Type::DOUBLE_QUOTE)) {
        //std::cout << "[DEBUG] Parsing double quoted string\n";
        children.push_back(parseDOUBLE_QUOTED_STRING());
    } else {
        Node* expr = parseEXPRESSION();
        if (!expr) {
            std::cerr << "[ERROR] parseEXPRESSION returned null!\n";
        } else {
            //std::cout << "[DEBUG] Parsed expression: " << expr->name << "\n";
        }
        children.push_back(expr);
    }
    //std::cout << "[DEBUG] Building InitializationExpression node\n";
    return buildNode("InitializationExpression", children);
}

Node* Parser::parseITERATION_ASSIGNMENT() {
    std::vector<Node*> children;
    // <IDENTIFIER>
    children.push_back(parseIDENTIFIER());
    // <ASSIGNMENT_OPERATOR>
    children.push_back(match(Type::ASSIGNMENT_OPERATOR));
    // <EXPRESSION> | <SINGLE_QUOTED_STRING> | <DOUBLE_QUOTED_STRING>
    if (check(Type::SINGLE_QUOTE)) {
        children.push_back(parseSINGLE_QUOTED_STRING());
    } else if (check(Type::DOUBLE_QUOTE)) {
        children.push_back(parseDOUBLE_QUOTED_STRING());
    } else {
        children.push_back(parseEXPRESSION());
    }
    return buildNode("IterationAssignment", children);
}

Node* Parser::parseASSIGNMENT_STATEMENT() {
    //std::cout << "[DEBUG] Entering parseASSIGNMENT_STATEMENT()\n";
    // <IDENTIFIER>
    if (!currentToken().isIdentifier()) {
        error("Syntax error on line " + std::to_string(currentToken().getLine()) +
              ": expected identifier in assignment.");
    }
    if (peekNext().getType() != Type::L_BRACKET) {
        //std::cout << "[DEBUG] Detected plain assignment\n";
        Node* initExpr = parseINITIALIZATION_EXPRESSION();
        if (!initExpr) {
            std::cerr << "[ERROR] parseINITIALIZATION_EXPRESSION returned nullptr\n";
        } else {
            //std::cout << "[DEBUG] Got initExpr node: " << initExpr->name << "\n";
        }
        if (currentToken().getType() != Type::SEMICOLON) {
            error("Syntax error on line " + std::to_string(currentToken().getLine()) +
                  ": expected ';' at end of assignment.");
        }
        Node* semi = match(Type::SEMICOLON);
        //std::cout << "[DEBUG] Matched semicolon\n";
        return buildNode("AssignmentStatement", { initExpr, semi });
    }
    //std::cout << "[DEBUG] Detected array assignment\n";
    std::vector<Node*> assignChildren;
    std::vector<Node*> arrayChildren;
    arrayChildren.push_back(parseIDENTIFIER());
    //std::cout << "[DEBUG] Parsed array identifier: " << arrayChildren.back()->name << "\n";
    arrayChildren.push_back(match(Type::L_BRACKET));
    //std::cout << "[DEBUG] Matched L_BRACKET\n";
    Node* indexExpr = parseNUMERICAL_EXPRESSION();
    if (!indexExpr) {
        std::cerr << "[ERROR] parseNUMERICAL_EXPRESSION returned nullptr!\n";
    }
    arrayChildren.push_back(indexExpr);
    //std::cout << "[DEBUG] Parsed index expression: " << indexExpr->name << "\n";
    arrayChildren.push_back(match(Type::R_BRACKET));
    //std::cout << "[DEBUG] Matched R_BRACKET\n";
    Node* arrayAccess = buildNode("ArrayAccess", arrayChildren);
    assignChildren.push_back(arrayAccess);
    assignChildren.push_back(match(Type::ASSIGNMENT_OPERATOR));
    //std::cout << "[DEBUG] Matched assignment operator\n";
    if (check(Type::SINGLE_QUOTE)) {
        //std::cout << "[DEBUG] Parsing single-quoted string\n";
        assignChildren.push_back(parseSINGLE_QUOTED_STRING());
    } else if (check(Type::DOUBLE_QUOTE)) {
        //std::cout << "[DEBUG] Parsing double-quoted string\n";
        assignChildren.push_back(parseDOUBLE_QUOTED_STRING());
    } else {
        Node* rhsExpr = parseEXPRESSION();
        if (!rhsExpr) {
            std::cerr << "[ERROR] parseEXPRESSION returned nullptr!\n";
        } else {
            //std::cout << "[DEBUG] Parsed expression: " << rhsExpr->name << "\n";
        }
        assignChildren.push_back(rhsExpr);
    }
    assignChildren.push_back(match(Type::SEMICOLON));
    //std::cout << "[DEBUG] Matched final semicolon\n";
    return buildNode("AssignmentStatement", assignChildren);
}


Node* Parser::parseDECLARATION_STATEMENT() {
    vector<Node*> children;
    Node* dtype = parseDATATYPE_SPECIFIER();
    children.push_back(dtype);
    Node* idOrList = nullptr;
    if (peekNext().getType() == Type::SEMICOLON) {
        idOrList = parseIDENTIFIER();
    } else {
        idOrList = parseIDENTIFIER_AND_IDENTIFIER_ARRAY_LIST();
    }
    children.push_back(idOrList);
    children.push_back(match(Type::SEMICOLON));
    return buildNode("DeclarationStatement", children);
}

Node* Parser::parseRETURN_STATEMENT() {
    std::vector<Node*> children;
    children.push_back(match(Type::RETURN_KEYWORD));
    if (check(Type::SINGLE_QUOTE)) {
        children.push_back(parseSINGLE_QUOTED_STRING());
    } else if (check(Type::DOUBLE_QUOTE)) {
        children.push_back(parseDOUBLE_QUOTED_STRING());
    } else {
        children.push_back(parseEXPRESSION());
    }
    children.push_back(match(Type::SEMICOLON));
    return buildNode("ReturnStatement", children);
}

Node* Parser::parseSELECTION_STATEMENT() {
    std::vector<Node*> children;
    // if
    children.push_back(match(Type::IF));
    // (
    children.push_back(match(Type::L_PAREN));
    // <BOOLEAN_EXPRESSION>
    children.push_back(parseBOOLEAN_EXPRESSION());
    // )
    children.push_back(match(Type::R_PAREN));
    // <STATEMENT> | <BLOCK_STATEMENT>
    Node* ifBranch = nullptr;
    if (check(Type::L_BRACE)) {
        ifBranch = parseBLOCK_STATEMENT();
    } else {
        ifBranch = parseSTATEMENT();
    }
    children.push_back(ifBranch);
    if (check(Type::ELSE)) {
        children.push_back(match(Type::ELSE));
        Node* elseBranch = nullptr;
        if (check(Type::L_BRACE)) {
            elseBranch = parseBLOCK_STATEMENT();
        } else {
            elseBranch = parseSTATEMENT();
        }
        children.push_back(elseBranch);
    }
    return buildNode("SelectionStatement", children);
}

Node* Parser::parsePRINTF_STATEMENT() {
    std::vector<Node*> children;
    children.push_back(match(Type::PRINTF));
    children.push_back(match(Type::L_PAREN));
    Node* formatString = nullptr;
    if (check(Type::DOUBLE_QUOTE)) {
        formatString = parseDOUBLE_QUOTED_STRING();
    } else if (check(Type::SINGLE_QUOTE)) {
        formatString = parseSINGLE_QUOTED_STRING();
    } else {
        error("Expected quoted string as format string in printf on line " + std::to_string(currentToken().getLine()));
    }
    children.push_back(formatString);
    if (check(Type::COMMA)) {
        children.push_back(match(Type::COMMA));
        children.push_back(parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST());
    }
    children.push_back(match(Type::R_PAREN));
    children.push_back(match(Type::SEMICOLON));
    return buildNode("PrintfStatement", children);
}

bool Parser::startsStatement(Type t) {
    return t == Type::IF ||
           t == Type::WHILE ||
               t == Type::FOR ||
           t == Type::RETURN_KEYWORD ||
           t == Type::PRINTF ||
           t == Type::IDENTIFIER ||
           t == Type::CHAR || t == Type::INT || t == Type::BOOL ||
           t == Type::L_BRACE;
}

/*Node* Parser::parseCOMPOUND_STATEMENT() {
    std::vector<Node*> children;
    // <STATEMENT>
    children.push_back(parseSTATEMENT());
    // if <STATEMENT>, then compound statement
    while (startsStatement(currentToken().getType())) {
        children.push_back(parseSTATEMENT());
    }
    return buildNode("CompoundStatement", children);
}*/

Node* Parser::parseCOMPOUND_STATEMENT() {
    std::vector<Node*> children;
    if (startsStatement(currentToken().getType())) {
        children.push_back(parseSTATEMENT());
        while (startsStatement(currentToken().getType())) {
            children.push_back(parseSTATEMENT());
        }
    }
    return buildNode("CompoundStatement", children);
}

Node* Parser::parseBLOCK_STATEMENT() {
    std::vector<Node*> children;
    children.push_back(match(Type::L_BRACE));
    children.push_back(parseCOMPOUND_STATEMENT());
    children.push_back(match(Type::R_BRACE));
    return buildNode("BlockStatement", children);
}

Node* Parser::parseITERATION_STATEMENT() {
    std::vector<Node*> children;
    std::string keyword = currentToken().getText();
    if (keyword == "for") {
        // for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> <BOOLEAN_EXPRESSION> <SEMICOLON> <ITERATION_ASSIGNMENT> <R_PAREN> <STATEMENT/BLOCK_STATEMENT>
        children.push_back(match(Type::FOR));
        children.push_back(match(Type::L_PAREN));
        children.push_back(parseINITIALIZATION_EXPRESSION());
        children.push_back(match(Type::SEMICOLON));
        children.push_back(parseBOOLEAN_EXPRESSION());
        children.push_back(match(Type::SEMICOLON));
        children.push_back(parseITERATION_ASSIGNMENT());
        children.push_back(match(Type::R_PAREN));
        // Decide if it's a simple statement or a block
        if (check(Type::L_BRACE)) {
            children.push_back(parseBLOCK_STATEMENT());
        } else {
            children.push_back(parseSTATEMENT());
        }
        return buildNode("IterationStatement", children);
    }
    if (keyword == "while") {
        // while <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT/BLOCK_STATEMENT>
        children.push_back(match(Type::WHILE));
        children.push_back(match(Type::L_PAREN));
        children.push_back(parseBOOLEAN_EXPRESSION());
        children.push_back(match(Type::R_PAREN));
        if (check(Type::L_BRACE)) {
            children.push_back(parseBLOCK_STATEMENT());
        } else {
            children.push_back(parseSTATEMENT());
        }
        return buildNode("IterationStatement", children);
    }
    error("Syntax error on line " + std::to_string(currentToken().getLine()) +
          ": expected 'for' or 'while' in iteration statement.");
    return nullptr;
}

Node* Parser::parseSTATEMENT() {
    Type t = currentToken().getType();
    if (t == Type::CHAR || t == Type::BOOL || t == Type::INT) {
        return parseDECLARATION_STATEMENT();
    }
    if (t == Type::RETURN_KEYWORD) {
        return parseRETURN_STATEMENT();
    }
    if (t == Type::PRINTF) {
        return parsePRINTF_STATEMENT();
    }
    if (t == Type::IF) {
        return parseSELECTION_STATEMENT();
    }
    if (t == Type::WHILE || t == Type::FOR) {
        return parseITERATION_STATEMENT();
    }
    if (t == Type::IDENTIFIER) {
        Token next = peekNext();
        if (next.getType() == Type::L_PAREN) {
            return parseUSER_DEFINED_PROCEDURE_CALL_STATEMENT();
        }
        if (next.getType() == Type::ASSIGNMENT_OPERATOR || next.getType() == Type::L_BRACKET) {
            return parseASSIGNMENT_STATEMENT();
        }
        error("Unrecognized statement starting with identifier: " + currentToken().getText());
    }
    error("Unexpected token in statement: " + currentToken().getText() +
          " on line " + std::to_string(currentToken().getLine()));
    return nullptr;
}

Node* Parser::parsePARAMETER_LIST() {
    std::vector<Node*> children;
    children.push_back(parseDATATYPE_SPECIFIER());
    Token next = peekNext();
    if (next.getType() == Type::COMMA) {
        Token afterComma = getLookahead(2);
        if (afterComma.getText() == "char" ||
            afterComma.getText() == "int" ||
            afterComma.getText() == "bool") {
            children.push_back(parseIDENTIFIER());
            } else {
                children.push_back(parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST_DECLARATION());
            }
    } else {
        if (next.getType() == Type::L_BRACKET) {
            children.push_back(parseIDENTIFIER_AND_IDENTIFIER_ARRAY_PARAMETER_LIST_DECLARATION());
        } else {
            children.push_back(parseIDENTIFIER());
        }
    }
    if (check(Type::COMMA)) {
        children.push_back(match(Type::COMMA));
        Node* rest = parsePARAMETER_LIST();
        children.push_back(rest);
    }
    return buildNode("ParameterList", children);
}

Node* Parser::parseFUNCTION_DECLARATION() {
    std::vector<Node*> children;
    // function keyword
    children.push_back(match(Type::FUNCTION));
    // return type
    children.push_back(parseDATATYPE_SPECIFIER());
    // function name
    children.push_back(parseIDENTIFIER());
    // (
    children.push_back(match(Type::L_PAREN));
    // void | <PARAMETER_LIST>
    if (check(Type::VOID)) {
        children.push_back(match(Type::VOID));
    } else {
        children.push_back(parsePARAMETER_LIST());
    }
    // )
    children.push_back(match(Type::R_PAREN));
    // {
    children.push_back(match(Type::L_BRACE));
    children.push_back(parseCOMPOUND_STATEMENT());
    // }
    children.push_back(match(Type::R_BRACE));
    return buildNode("FunctionDeclaration", children);
}

Node* Parser::parsePROCEDURE_DECLARATION() {
    std::vector<Node*> children;
    // procedure
    children.push_back(match(Type::PROCEDURE));
    // <IDENTIFIER>
    children.push_back(parseIDENTIFIER());
    // <L_PAREN>
    children.push_back(match(Type::L_PAREN));
    // void | <PARAMETER_LIST>
    if (check(Type::VOID)) {
        children.push_back(match(Type::VOID));
    } else {
        children.push_back(parsePARAMETER_LIST());
    }
    // <R_PAREN>
    children.push_back(match(Type::R_PAREN));
    // <L_BRACE>
    children.push_back(match(Type::L_BRACE));
    children.push_back(parseCOMPOUND_STATEMENT());
    // <R_BRACE>
    children.push_back(match(Type::R_BRACE));
    return buildNode("ProcedureDeclaration", children);
}

Node* Parser::parseMAIN_PROCEDURE() {
    std::vector<Node*> children;
    children.push_back(match(Type::PROCEDURE));
    if (currentToken().getType() != Type::IDENTIFIER || currentToken().getText() != "main") {
        error("Expected 'main' identifier after 'procedure' on line " +
              std::to_string(currentToken().getLine()));
    }
    children.push_back(match(Type::IDENTIFIER));
    children.push_back(match(Type::L_PAREN));
    children.push_back(match(Type::VOID));
    children.push_back(match(Type::R_PAREN));
    children.push_back(parseBLOCK_STATEMENT());
    return buildNode("MainProcedure", children);
}

Node* Parser::parsePROGRAM_TAIL() {
    std::vector<Node*> children;
    while (true) {
        Type t = currentToken().getType();
        if (t == Type::FUNCTION) {
            children.push_back(parseFUNCTION_DECLARATION());
        }
        else if (t == Type::PROCEDURE) {
            Token next = peekNext();
            if (next.getType() == Type::IDENTIFIER && next.getText() == "main") {
                break;
            }
            children.push_back(parsePROCEDURE_DECLARATION());
        }
        else if (t == Type::CHAR || t == Type::BOOL || t == Type::INT) {
            children.push_back(parseDECLARATION_STATEMENT());
        }
        else {
            break;
        }
    }
    return buildNode("ProgramTail", children);
}

Node* Parser::parsePROGRAM() {
    std::vector<Node*> children;
    Type t = currentToken().getType();
    if (t == Type::PROCEDURE) {
        Token next = peekNext();
        if (next.getType() == Type::IDENTIFIER && next.getText() == "main") {
            children.push_back(parseMAIN_PROCEDURE());
            if (!check(Type::END_OF_FILE)) {
                children.push_back(parsePROGRAM_TAIL());
            }
            return buildNode("Program", children);
        }
        children.push_back(parsePROCEDURE_DECLARATION());
        children.push_back(parsePROGRAM());
        return buildNode("Program", children);
    }
    if (t == Type::FUNCTION) {
        children.push_back(parseFUNCTION_DECLARATION());
        children.push_back(parsePROGRAM());
        return buildNode("Program", children);
    }
    if (t == Type::CHAR || t == Type::BOOL || t == Type::INT) {
        children.push_back(parseDECLARATION_STATEMENT());
        children.push_back(parsePROGRAM());
        return buildNode("Program", children);
    }
    error("Unexpected token '" + currentToken().getText() +
          "' at line " + std::to_string(currentToken().getLine()) +
          ". Expected function, procedure, declaration, or main procedure.");
    return nullptr;
}

//Output Functions:
// Prints entire recursively created LCRS tree with all derivations
void Parser::graphicPrintTree(Node* root) {
    printTreeHelper(root, "", true);
}

void Parser::printTreeHelper(Node* node, std::string prefix, bool isLastChild) {
    if (node == nullptr) return;
    // Print branch prefix
    std::cout << prefix;
    std::cout << (isLastChild ? "+-- " : "|-- ");
    // Print node name and line number
    std::cout << node->name;
    if (node->lineNumber > 0) {
        std::cout << " (" << node->lineNumber << ")";
    }
    std::cout << std::endl;
    // Update branch prefix
    std::string childPrefix = prefix + (isLastChild ? "    " : "|   ");
    // Count children and print
    int numChildren = 0;
    Node* temp = node->leftChild;
    while (temp) {
        numChildren++;
        temp = temp->rightSibling;
    }
    temp = node->leftChild;
    int childIndex = 0;
    while (temp) {
        printTreeHelper(temp, childPrefix, ++childIndex == numChildren);
        temp = temp->rightSibling;
    }
}

void Parser::collectTerminalNodes(Node* node, vector<Node*>& terminals) {
    if (node == nullptr) {
        return;
    }
    if (node->leftChild == nullptr) {
        if (node->name != "CompoundStatement") {
            terminals.push_back(node);
        }
    }
    collectTerminalNodes(node->leftChild, terminals);
    collectTerminalNodes(node->rightSibling, terminals);
}

Node* Parser::makeTerminalOnlyCST(Node* parseTreeRoot) {
    vector<Node*> terminals;
    collectTerminalNodes(parseTreeRoot, terminals);
    map<int, vector<Node*>> tokensByLine;
    for (Node* token : terminals) {
        Node* cloned = token->clone();
        tokensByLine[token->lineNumber].push_back(cloned);
    }
    Node* newRoot = nullptr;
    Node* prevLastToken = nullptr;
    for (auto& [line, lineTokens] : tokensByLine) {
        for (int i = 1; i < lineTokens.size(); ++i) {
            lineTokens[i - 1]->rightSibling = lineTokens[i];
        }
        if (!lineTokens.empty()) {
            lineTokens.back()->rightSibling = nullptr;
        }
        if (newRoot == nullptr && !lineTokens.empty()) {
            newRoot = lineTokens[0];
        }
        if (prevLastToken && !lineTokens.empty()) {
            prevLastToken->leftChild = lineTokens[0];
        }
        if (!lineTokens.empty()) {
            prevLastToken = lineTokens.back();
        }
    }
    return newRoot;
}

/*void Parser::writeTerminalsOnly(Node* root, std::ostream& out) {
    map<int, vector<string>> lineToTerminals;
    collectTerminals(root, lineToTerminals);
    int previousLineLength = 0;
    for (auto it = lineToTerminals.begin(); it != lineToTerminals.end(); ++it) {
        const auto& terminals = it->second;
        std::string line;
        // align with last token
        line += std::string(previousLineLength, ' ');
        // make current line
        std::string content;
        for (int i = 0; i < terminals.size(); ++i) {
            content += terminals[i];
            if (i + 1 < terminals.size()) content += " -> ";
        }
        content += " -> NULL";
        line += content;
        out << line << std::endl;
        // update alignment before print
        previousLineLength += (content.length() - 9);
        // print '|' where next line starts
        auto nextIt = next(it);
        if (nextIt != lineToTerminals.end()) {
            std::string connector(previousLineLength, ' ');
            connector += '|';
            out << connector << std::endl;
        }
    }
}*/