
#ifndef NODE_H
#define NODE_H

#include <string>
#include <iostream>

using namespace std;

struct Node {
    static inline int globalID = 0;
    int id;
    string name;
    int lineNumber;
    Node* leftChild;
    Node* rightSibling;

    Node(const std::string &n, int line = 0)
        : name(n), lineNumber(line), leftChild(nullptr), rightSibling(nullptr) {
        id = globalID++;
    }

    void printTree(int indent = 0) const {
        std::cout << std::string(indent, ' ') << name << " (line " << lineNumber << ")\n";
        if (leftChild) {
            leftChild->printTree(indent + 2);
        }
        if (rightSibling) {
            rightSibling->printTree(indent);
        }
    }

    Node* clone() const {
        Node* copy = new Node(this->name, this->lineNumber);
        // Optionally copy ID if needed
        copy->id = this->id;
        return copy;
    }
};

// Add a child node to lcrs tree
inline void addChild(Node* parent, Node* child) {
    if (!parent || !child) return;
    if (!parent->leftChild) {
        parent->leftChild = child;
    } else {
        Node* tmp = parent->leftChild;
        while (tmp->rightSibling != nullptr) {
            tmp = tmp->rightSibling;
        }
        tmp->rightSibling = child;
    }
}

// Add a sibling node to the lcrs tree
inline void addSibling(Node* leftNode, Node* sibling) {
    if (!leftNode || !sibling) return;
    while (leftNode->rightSibling) {
        leftNode = leftNode->rightSibling;
    }
    leftNode->rightSibling = sibling;
}

#endif