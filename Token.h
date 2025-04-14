/** ***************************************************************************
* @remark CS460: Programming Assignment 2 - Tokenization                      *
 * @remark C++ implementation of a deterministic finite state automaton used  *
 *          to tokenize input from C-style code.                              *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  Token.h                                                             *
 * @date  February 25, 2025                                                   *
 *****************************************************************************/

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

//DFA States
enum class Type {
    IDENTIFIER,
    L_PAREN, R_PAREN,
    L_BRACE, R_BRACE,
    L_BRACKET, R_BRACKET,
    SEMICOLON,
    ASSIGNMENT_OPERATOR,
    DOUBLE_QUOTE, SINGLE_QUOTE,
    STRING,
    INTEGER,
    COMMA,
    MODULO,
    BOOLEAN_EQUAL, BOOLEAN_AND, BOOLEAN_NOT_EQUAL, BOOLEAN_OR, BOOLEAN_NOT,
    BOOLEAN_TRUE, BOOLEAN_FALSE,
    ASTERISK, PLUS, MINUS, DIVIDE,
    LT_EQUAL, GT_EQUAL, LT, GT,
    CARET,
    RETURN_KEYWORD,
    CHAR, BOOL, INT,
    IF, ELSE, WHILE, FOR,
    PRINTF, FUNCTION, PROCEDURE,
    GETCHAR, VOID, SIZEOF,
    DOUBLE_QUOTED_STRING, SINGLE_QUOTED_STRING,
    //Error States
    ERROR_INVALID_INT, ERROR_INVALID_IDENTIFIER, ERROR_UNCLOSED_STRING,
    ERROR_UNCLOSED_CHAR, ERROR_INVALID_CHAR, ERROR_INVALID_OPERATOR,
    END_OF_FILE
};

class Token {
public:
    Token(Type type, const string &text, int line);

    Type getType() const { return _type; }
    string getText() const { return _text; }
    int getLine() const { return _line; }
    string getTypeName() const;
    bool isEndOfFile() const;

    bool isIdentifier() const;
    bool isInteger() const;
    bool isString() const;
    bool isOperator() const;
    bool isBoolean() const;
    bool isChar() const;
    bool isDelimiter() const;
    bool isError() const;

private:
    Type _type;
    string _text;
    int _line;
};


#endif //TOKEN_H
