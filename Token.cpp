/** ***************************************************************************
* @remark CS460: Programming Assignment 2 - Tokenization                      *
 * @remark C++ implementation of a deterministic finite state automaton used  *
 *          to tokenize input from C-style code.                              *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  Token.cpp                                                           *
 * @date  February 25, 2025                                                   *
 *****************************************************************************/

#include "Token.h"

Token::Token(Type type, const string &text, int line) {
    _type = type;
    _text = text;
    _line = line;
}

bool Token::isIdentifier() const {
    return _type == Type::IDENTIFIER;
}

bool Token::isInteger() const {
    return _type == Type::INTEGER;
}

bool Token::isString() const {
    return  _type == Type::STRING ||
            _type == Type::DOUBLE_QUOTED_STRING ||
            _type == Type::SINGLE_QUOTED_STRING;
}

bool Token::isEndOfFile() const {
    return _type == Type::END_OF_FILE;
}

string Token::getTypeName() const {
    switch (_type) {
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

bool Token::isOperator() const {
    return _type == Type::ASSIGNMENT_OPERATOR || _type == Type::BOOLEAN_EQUAL ||
           _type == Type::BOOLEAN_NOT_EQUAL || _type == Type::BOOLEAN_AND ||
           _type == Type::BOOLEAN_OR || _type == Type::PLUS ||
           _type == Type::MINUS || _type == Type::ASTERISK ||
           _type == Type::DIVIDE || _type == Type::MODULO ||
           _type == Type::LT || _type == Type::GT ||
           _type == Type::LT_EQUAL || _type == Type::GT_EQUAL;
}

bool Token::isBoolean() const {
    return _type == Type::BOOLEAN_TRUE || _type == Type::BOOLEAN_FALSE;
}

bool Token::isChar() const {
    return _type == Type::SINGLE_QUOTE;
}

bool Token::isDelimiter() const {
    return _type == Type::SEMICOLON || _type == Type::COMMA;
}

bool Token::isError() const {
    return     _type == Type::ERROR_INVALID_INT ||
               _type == Type::ERROR_INVALID_IDENTIFIER ||
               _type == Type::ERROR_UNCLOSED_STRING ||
               _type == Type::ERROR_UNCLOSED_CHAR ||
               _type == Type::ERROR_INVALID_CHAR || _type == Type::ERROR_INVALID_OPERATOR;
}