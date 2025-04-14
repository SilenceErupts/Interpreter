
#include "TokenList.h"
#include "Token.h"
#include <iostream>

TokenList::TokenList() : head(nullptr), tail(nullptr), _size(0) {}

TokenList::~TokenList() {
    while (head != nullptr) {
        TokenNode* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    _size = 0;
}

void TokenList::push_back(const Token &t) {
    TokenNode* newNode = new TokenNode(t);
    if (!head) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    _size++;
}

Token TokenList::front() const {
    if (!head) {
        return Token(Type::END_OF_FILE, "", 0);
    }
    return head->token;
}

void TokenList::pop_front() {
    if (!head) return;
    TokenNode* temp = head;
    head = head->next;
    if (!head) {
        tail = nullptr;
    }
    delete temp;
    _size--;
}

bool TokenList::empty() const {
    return (head == nullptr);
}

size_t TokenList::size() const {
    return _size;
}

void TokenList::printAllTokens() const {
    TokenNode* cur = head;
    while (cur) {
        std::cout
          << "Token type: " << cur->token.getTypeName() << "\n"
          << "Token text: " << cur->token.getText() << "\n"
          << "Line:       " << cur->token.getLine() << "\n\n";
        cur = cur->next;
    }
}

Token TokenList::peekNext() const {
    if (!head || !head->next) {
        return Token(Type::END_OF_FILE, "", -1);
    }
    return head->next->token;
}

void TokenList::push_front(const Token& t) {
    TokenNode* node = new TokenNode(t);
    node->next = head;
    head = node;
    if (tail == nullptr) {
        tail = node;
    }
    ++_size;
}
