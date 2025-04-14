
#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "Token.h"

using namespace std;

struct TokenNode {
    Token token;
    TokenNode* next;

    TokenNode(const Token& t) : token(t), next(nullptr) {}
};

class TokenList {
public:
    TokenList();
    ~TokenList();
    TokenNode* getHead() const {return head;}
    // Add a token to the end of the list
    void push_back(const Token &t);
    void push_front(const Token& t);
    // get front token without removing it
    Token front() const;
    // remove front token
    void pop_front();
    bool empty() const;
    size_t size() const;
    // Debug print
    void printAllTokens() const;
    Token peekNext() const;

private:
    TokenNode* head;
    TokenNode* tail;
    size_t     _size;
};

#endif // TOKENLIST_H
