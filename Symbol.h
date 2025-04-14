/** ***************************************************************************
* @remark CS460: Programming Assignment 4 - Symbol Table                      *
 *                                                                            *
 * @author Sam Tyler                                                          *
 * @file  Symbol.h                                                            *
 * @date  April 8, 2025                                                       *
 *****************************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
using namespace std;

struct Symbol {
    string name;
    string idType;       // datatype, function, or procedure
    string dataType;     // variables - datatype; functions - return type; procedures - NOT APPLICABLE
    int scope;
    bool isArray;
    int arraySize;
    int line;
    vector<Symbol> parameters;
};

#endif //SYMBOL_H
