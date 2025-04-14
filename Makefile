a.out:
	g++ -std=c++20 Token.h Token.cpp Tokenizer.h Tokenizer.cpp IgnoreComments.cpp Node.h Parser.cpp Parser.h TokenList.cpp TokenList.h Symbol.h SymbolTable.h SymbolTable.cpp main.cpp -o a.out

clean:
	rm -f a.out