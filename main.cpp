/** ***************************************************************************
* @remark CS460: Programming Assignment 4 - Symbol Table                      *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  main.cpp                                                            *
 * @date  April 8, 2025                                                       *
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include "Tokenizer.h"
#include "TokenList.h"
#include "Parser.h"
#include "Node.h"
#include "SymbolTable.h"

void IgnoreComments(const std::string &inputFile, const std::string &preprocessedFile);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <inputFile>\n";
        return 1;
    }

    // Remove Comments
    string inputFile = argv[1];
    //string inputFile = "programming_assignment_4-test_file_2.c";
    string preprocessedFile = "nocomment_input.txt";
    IgnoreComments(inputFile, preprocessedFile);

    // Create tokenizer
    Tokenizer tokenizer(preprocessedFile);
    TokenList tokens;
    bool foundError = false;
    string errorMsg;

    // Read file and store tokens
    while (true) {
        Token token = tokenizer.getToken();
        if (token.isEndOfFile()) {
            tokens.push_back(token);
            break;
        }
        if (token.isError()) {
            foundError = true;
            // create tokenizer error message
            switch (token.getType()) {
                case Type::ERROR_INVALID_INT:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": invalid integer\n";
                    break;
                case Type::ERROR_UNCLOSED_STRING:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": unterminated string quote\n";
                    break;
                case Type::ERROR_UNCLOSED_CHAR:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": unclosed character literal\n";
                    break;
                case Type::ERROR_INVALID_CHAR:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": invalid character\n";
                break;
                case Type::ERROR_INVALID_OPERATOR:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": invalid operator\n";
                break;
                case Type::ERROR_INVALID_IDENTIFIER:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": invalid identifier\n";
                break;
                default:
                    errorMsg = "Syntax error on line " + std::to_string(token.getLine()) + ": unknown error\n";
                break;
            }
            break;
        }
        tokens.push_back(token);
    }
    if (foundError) {
        std::cout << errorMsg << std::endl;
        exit(1);
    }

    //Debug Print:
    //std::cout << "\nCollected " << tokens.size() << " tokens.\n";
    //tokens.printAllTokens();

    //Begin Recursive Descent Parsing (Create CST)
    Parser parser(tokens);
    Node* CST = parser.parse();

    //parser.graphicPrintTree(CST);
    //root->printTree();
    //Node* terminalCST = parser.makeTerminalOnlyCST(CST);
    //terminalCST->printTree();

    //Create Symbol Table
    SymbolTable symbolTable;
    traverseCST(CST, 0, symbolTable);
    if (!symbolError) {
        symbolTable.print();
    }

    /*if (root) {
        std::ofstream outFile("cst_output.txt");
        if (!outFile.is_open()) {
            std::cerr << "Error: could not open cst_output.txt for writing." << std::endl;
            return 1;
        }
        parser.writeTerminalsOnly(root, outFile);
        parser.printTree(root);
        outFile.close();
    }*/
    return 0;
}
