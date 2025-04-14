/** ***************************************************************************
* @remark CS460: Programming Assignment 2 - Tokenization                      *
 * @remark C++ implementation of a deterministic finite state automaton used  *
 *          to tokenize input from C-style code.                              *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  Tokenizer.h                                                         *
 * @date  February 25, 2025                                                   *
 *****************************************************************************/

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <fstream>
#include <deque>
#include "Token.h"

class Tokenizer {
public:
    explicit Tokenizer(const std::string &inputFile);
    Token getToken();

private:
    std::ifstream inputStream;
    std::ofstream outputStream;
    int lineNum = 1;
    std::deque<Token> _pendingTokens;

    // Skip whitespace (includes '\r', '\n', ...)
    void skipWhitespace();
    // Helpers for token types
    void buildDoubleQuoteTokens();
    void buildSingleQuoteTokens();
    Token readIdentifierOrKeyword(char firstChar);
    Token readNumber(char firstChar);
    Token readOperator(char firstChar);
    Token readNegativeNumber();
    //Cast helper
    static unsigned char toUnsignedChar(char c);

};

#endif // TOKENIZER_H
