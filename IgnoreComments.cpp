/** ***************************************************************************
 * @remark CS460: Programming Assignment 1 - Ignore Comments                  *
 * @remark C++ implementation of a deterministic finite state automaton used  *
 *          to remove line and block comments from C-style code.              *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  IgnoreComments.cpp                                                  *
 * @date  February 13, 2025                                                   *
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//DFA States
enum State { NORMAL, // Not inside any comment
    COMMENT_START, // Have seen a '/' and waiting for next character
    LINE, // Inside a line comment
    BLOCK, // Inside a block comment
    BLOCK_END, // Saw a '*' inside a block comment, checking for '/'
    STRING // Inside a double-quoted string
    };

void IgnoreComments(const std::string &inputFile, const std::string &outputFile) {
    ifstream fIn(inputFile);
    if (!fIn) {
        cerr << "Error: Could not open file " << inputFile << endl;
        exit(1);
    }

    ofstream fOut(outputFile);
    if (!fOut) {
        cerr << "Error: Could not open output file " << outputFile << endl;
        exit(1);
    }


    string output;
    State curState = NORMAL;
    int curLine = 1;
    int blockStart = 0;  // For errors, if a block comment is unterminated
    char curChar;

    while (fIn.get(curChar)) {
        switch (curState) {
            case NORMAL:
                // Case when entering a string
                if (curChar == '"') {
                    output.push_back(curChar);
                    curState = STRING;
                }
                // Case when starting a comment
                else if (curChar == '/') {
                    curState = COMMENT_START;
                }
                // Case for stray ending block comment, need to see next char
                else if (curChar == '*') {
                    if (fIn.peek() == '/') {
                        cerr << "ERROR: Program contains C-style, unterminated comment on line "
                            << curLine << endl;
                        //return;
                        exit(1);
                    }
                    output.push_back(curChar);
                }
                // "Normal" characters
                else {
                    output.push_back(curChar);
                }
            if (curChar == '\n') {
                curLine++;
            }
            break;

            case COMMENT_START:
                //Case for Line comment
                if (curChar == '/') {
                    output.append("  ");
                    curState = LINE;
                }
                //Case for block comment
                else if (curChar == '*') {
                    blockStart = curLine;
                    output.append("  ");
                    curState = BLOCK;
                }
                //Not a comment
                else {
                    output.push_back('/');
                    output.push_back(curChar);
                    if (curChar == '\n') {
                        curLine++;
                    }
                    curState = NORMAL;
                }
            break;

            case LINE:
                // Replaces all remaining characters in the line with white space
                if (curChar == '\n') {
                    output.push_back('\n');
                    curLine++;
                    curState = NORMAL;
                }
                else {
                    output.push_back(' ');
                }
            break;

            case BLOCK:
                // Case where we find a '*'
                if (curChar == '*') {
                    output.push_back(' ');
                    curState = BLOCK_END;
                }
                //Still inside comment
                else {
                    if (curChar == '\n') {
                        output.push_back('\n');
                        curLine++;
                    }
                    else {
                        output.push_back(' ');
                    }
                }
            break;

            case BLOCK_END:
                //Case where the block comment ends
                if (curChar == '/') {
                    output.push_back(' ');
                    curState = NORMAL;
                }
                //Case where '*' is repeated
                else if (curChar == '*') {
                    output.push_back(' ');
                }
                else {
                    // Still inside comment
                    if (curChar == '\n') {
                        output.push_back('\n');
                        curLine++;
                    }
                    else {
                        output.push_back(' ');
                    }
                    curState = BLOCK;
                }
            break;

            case STRING:
                //output all characters
                output.push_back(curChar);
                // Case for escape characters in a string
                if (curChar == '\\') {
                    if (fIn.get(curChar)) {
                        output.push_back(curChar);
                        if (curChar == '\n')
                            curLine++;
                    }
                }
                //end of string
                else if (curChar == '"') {
                    curState = NORMAL;
                }
                else if (curChar == '\n') {
                    curLine++;
                }
            break;
        }
    }

    //Edge Cases:
    // Reaching end-of-file while in COMMENT_START, output pending '/'
    if (curState == COMMENT_START) {
        output.push_back('/');
    }
    // Reaching end-of-file while in a block comment
    if (curState == BLOCK || curState == BLOCK_END) {
        cerr << "ERROR: Program contains C-style, unterminated comment on line "
             << blockStart << endl;
        //return;
        exit(1);
    }

    fOut << output;
    fIn.close();
    fOut.close();
}