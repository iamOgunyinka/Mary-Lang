#include "Scanner/Scanner.hpp"
#include <iostream>

int main()
{
	Mary::Lexer::Scanner scanner( "file.axd" );
	Mary::Lexer::Token token = scanner.getNextToken();
	while( token.type() != Mary::Lexer::TokenType::TK_EOF 
		&& token.type() != Mary::Lexer::TokenType::TK_INVALID )
	{
		std::wcout << token << std::endl;
		token = scanner.getNextToken();
	}
	return 0;
}