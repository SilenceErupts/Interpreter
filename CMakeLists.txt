cmake_minimum_required(VERSION 3.30)
project(Interpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(Interpreter main.cpp
        Token.cpp
        Token.h
        Tokenizer.cpp
        Tokenizer.h
        IgnoreComments.cpp
        TokenList.cpp
        TokenList.h
        Parser.cpp
        Parser.h
        Node.h
        Parser.cpp
        Parser.h
        Symbol.h
        SymbolTable.cpp
        SymbolTable.h
)
