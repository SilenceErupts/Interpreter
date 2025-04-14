/** ***************************************************************************
* @remark CS460: Programming Assignment 2 - Tokenization                      *
 * @remark C++ implementation of a deterministic finite state automaton used  *
 *          to tokenize input from C-style code.                              *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  Tokenizer.cpp                                                       *
 * @date  February 25, 2025                                                   *
 *****************************************************************************/

#include "Tokenizer.h"
#include <iostream>
#include <cctype>

Tokenizer::Tokenizer(const string &inputFile) {
    inputStream.open(inputFile);
    if (!inputStream.is_open()) {
        cerr << "Error: Could not open file " << inputFile << std::endl;
        exit(1);
    }
}

unsigned char Tokenizer::toUnsignedChar(char c) {
    return static_cast<unsigned char>(c);
}

void Tokenizer::skipWhitespace() {
    char c;
    while (true) {
        if (!inputStream.get(c)) {
            return; // No more input
        }
        // Treat '\r' as whitespace
        if (c == '\r' || isspace(toUnsignedChar(c))) {
            if (c == '\n') {
                lineNum++;
            }
        }
        else { // Not whitespace, put back
            inputStream.unget();
            return;
        }
    }
}

Token Tokenizer::getToken() {
    // Use any former pending tokens
    if (!_pendingTokens.empty()) {
        Token t = _pendingTokens.front();
        _pendingTokens.pop_front();
        return t;
    }

    skipWhitespace();
    char c;

    //Case for End of File
    if (!inputStream.get(c)) {
        return Token(Type::END_OF_FILE, "", lineNum);
    }
    // Case for entering a string ("")
    if (c == '"') {
        buildDoubleQuoteTokens();
        if (!_pendingTokens.empty()) {
            Token t = _pendingTokens.front();
            _pendingTokens.pop_front();
            return t;
        }
        return Token(Type::END_OF_FILE, "", lineNum);
    }
    // Case for entering character literal ('')
    if (c == '\'') {
        buildSingleQuoteTokens();
        if (!_pendingTokens.empty()) {
            Token t = _pendingTokens.front();
            _pendingTokens.pop_front();
            return t;
        }
        return Token(Type::END_OF_FILE, "", lineNum);
    }
    // Case for letters (Includes '_')
    if (isalpha(toUnsignedChar(c)) || c == '_') {
        return readIdentifierOrKeyword(c);
    }
    // Case for digits
    if (isdigit(toUnsignedChar(c))) {
        return readNumber(c);
    }
    // Case for punctuation (Ex: ! for not, ? for turnary operator)
    if (ispunct(toUnsignedChar(c))) {
        return readOperator(c);
    }
    // Else unknown
    return Token(Type::ERROR_INVALID_CHAR, string(1, c), lineNum);
}

void Tokenizer::buildDoubleQuoteTokens() {
    _pendingTokens.push_back(Token(Type::DOUBLE_QUOTE, "\"", lineNum));
    std::string content;
    char c;
    bool escape = false;
    while (true) {
        if (!inputStream.get(c)) {
            // End of file before closing quote
            _pendingTokens.push_back(Token(Type::ERROR_UNCLOSED_STRING, content, lineNum));
            return;
        }
        if (escape) {
            // Add escaped character and reset flag
            content.push_back(c);
            escape = false;
            continue;
        }
        if (c == '\\') {
            // Escape next character
            escape = true;
            content.push_back(c);
            continue;
        }
        if (c == '"') {
            // Found unescaped closing quote
            break;
        }
        if (c == '\n' || c == '\r') {
            // Newline without closing quote
            inputStream.unget(); // Return newline for proper line counting
            _pendingTokens.push_back(Token(Type::ERROR_UNCLOSED_STRING, content, lineNum));
            return;
        }
        content.push_back(c);
    }
    _pendingTokens.push_back(Token(Type::STRING, content, lineNum));
    _pendingTokens.push_back(Token(Type::DOUBLE_QUOTE, "\"", lineNum));
}

void Tokenizer::buildSingleQuoteTokens() {
    _pendingTokens.push_back(Token(Type::SINGLE_QUOTE, "\'", lineNum));
    std::string content;
    char c;
    bool escape = false;
    while (true) {
        if (!inputStream.get(c)) {
            _pendingTokens.push_back(Token(Type::ERROR_UNCLOSED_CHAR, content, lineNum));
            return;
        }
        if (escape) {
            content.push_back(c);
            escape = false;
            continue;
        }
        if (c == '\\') {
            escape = true;
            content.push_back(c);
            continue;
        }
        if (c == '\'') {
            break; // Valid closing quote
        }
        if (c == '\n' || c == '\r') {
            inputStream.unget(); // Return newline for proper line counting
            _pendingTokens.push_back(Token(Type::ERROR_UNCLOSED_CHAR, content, lineNum));
            return;
        }
        content.push_back(c);
    }
    _pendingTokens.push_back(Token(Type::STRING, content, lineNum));
    _pendingTokens.push_back(Token(Type::SINGLE_QUOTE, "\'", lineNum));
}

Token Tokenizer::readIdentifierOrKeyword(char firstChar) {
    std::string inputIden(1, firstChar);
    char c;
    while (inputStream.get(c)) {
        if (std::isalnum(toUnsignedChar(c)) || c == '_') {
            inputIden.push_back(c);
        } else {
            inputStream.unget();
            break;
        }
    }
    // Specific Identifiers
    // Check for reserved keywords
    if (inputIden == "return")
        return Token(Type::RETURN_KEYWORD, inputIden, lineNum);
    if (inputIden == "TRUE")
        return Token(Type::BOOLEAN_TRUE, inputIden, lineNum);
    if (inputIden == "FALSE")
        return Token(Type::BOOLEAN_FALSE, inputIden, lineNum);
    if (inputIden == "char")
        return Token(Type::CHAR, inputIden, lineNum);
    if (inputIden == "bool")
        return Token(Type::BOOL, inputIden, lineNum);
    if (inputIden == "int")
        return Token(Type::INT, inputIden, lineNum);
    if (inputIden == "getchar")
        return Token(Type::GETCHAR, inputIden, lineNum);
    if (inputIden == "void")
        return Token(Type::VOID, inputIden, lineNum);
    if (inputIden == "sizeof")
        return Token(Type::SIZEOF, inputIden, lineNum);
    if (inputIden == "if")
        return Token(Type::IF, inputIden, lineNum);
    if (inputIden == "else")
        return Token(Type::ELSE, inputIden, lineNum);
    if (inputIden == "while")
        return Token(Type::WHILE, inputIden, lineNum);
    if (inputIden == "for")
        return Token(Type::FOR, inputIden, lineNum);
    if (inputIden == "printf")
        return Token(Type::PRINTF, inputIden, lineNum);
    if (inputIden == "function")
         return Token(Type::FUNCTION, inputIden, lineNum);
    if (inputIden == "procedure")
        return Token(Type::PROCEDURE, inputIden, lineNum);
    return Token(Type::IDENTIFIER, inputIden, lineNum);
}

Token Tokenizer::readNumber(char firstChar) {
    std::string inputNum(1, firstChar);
    char c;
    while (inputStream.get(c)) {
        if (std::isdigit(toUnsignedChar(c))) {
            inputNum.push_back(c);
        }
        else if (std::isalpha(toUnsignedChar(c))) {
            //Error Case
            inputNum.push_back(c);
            return Token(Type::ERROR_INVALID_INT, inputNum, lineNum);
        }
        else {
            inputStream.unget();
            break;
        }
    }
    return Token(Type::INTEGER, inputNum, lineNum);
}

Token Tokenizer::readNegativeNumber() {
    std::string inputNum("-");
    char c;
    // read char after '-'
    if (!inputStream.get(c)) {
        // No input
        return Token(Type::MINUS, "-", lineNum);
    }
    // Case for non-digit
    if (!std::isdigit(toUnsignedChar(c))) {
        inputStream.unget();
        return Token(Type::MINUS, "-", lineNum);
    }
    // Case for digits
    inputNum.push_back(c);
    while (inputStream.get(c)) {
        if (std::isdigit(toUnsignedChar(c))) {
            inputNum.push_back(c);
        }
        else if (std::isalpha(toUnsignedChar(c))) {
            //Error Case
            inputNum.push_back(c);
            return Token(Type::ERROR_INVALID_INT, inputNum, lineNum);
        }
        else {
            inputStream.unget();
            break;
        }
    }
    return Token(Type::INTEGER, inputNum, lineNum);
}

Token Tokenizer::readOperator(char firstChar) {
    std::string inputOp(1, firstChar);
    char c;
    if (inputStream.get(c)) {
        if (c == '\r') {
            inputStream.unget();
        } else {
            //Two character operators
            inputOp.push_back(c);
            if (inputOp == "==") return Token(Type::BOOLEAN_EQUAL, inputOp, lineNum);
            if (inputOp == "!=") return Token(Type::BOOLEAN_NOT_EQUAL, inputOp, lineNum);
            if (inputOp == "<=") return Token(Type::LT_EQUAL, inputOp, lineNum);
            if (inputOp == ">=") return Token(Type::GT_EQUAL, inputOp, lineNum);
            if (inputOp == "&&") return Token(Type::BOOLEAN_AND, inputOp, lineNum);
            if (inputOp == "||") return Token(Type::BOOLEAN_OR, inputOp, lineNum);
            inputStream.unget();
            inputOp.pop_back();
        }
    }
    // Single-character operators
    switch (firstChar) {
        case '-': {
            if (std::isdigit(toUnsignedChar(inputStream.peek()))) {
                return readNegativeNumber();
            }
            else {
                return Token(Type::MINUS, "-", lineNum);
            }
        }
        case '=':  return Token(Type::ASSIGNMENT_OPERATOR, "=", lineNum);
        case '!':  return Token(Type::BOOLEAN_NOT, "!", lineNum);
        case '+':  return Token(Type::PLUS, "+", lineNum);
        case '*':  return Token(Type::ASTERISK, "*", lineNum);
        case '/':  return Token(Type::DIVIDE, "/", lineNum);
        case '%':  return Token(Type::MODULO, "%", lineNum);
        case '<':  return Token(Type::LT, "<", lineNum);
        case '>':  return Token(Type::GT, ">", lineNum);
        case ';':  return Token(Type::SEMICOLON, ";", lineNum);
        case ',':  return Token(Type::COMMA, ",", lineNum);
        case '{':  return Token(Type::L_BRACE, "{", lineNum);
        case '}':  return Token(Type::R_BRACE, "}", lineNum);
        case '(':  return Token(Type::L_PAREN, "(", lineNum);
        case ')':  return Token(Type::R_PAREN, ")", lineNum);
        case '[':  return Token(Type::L_BRACKET, "[", lineNum);
        case ']':  return Token(Type::R_BRACKET, "]", lineNum);
        default:
            return Token(Type::ERROR_INVALID_OPERATOR, std::string(1, firstChar), lineNum);
    }
}
