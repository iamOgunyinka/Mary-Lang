#include "Scanner/Scanner.hpp"
#include <iostream>

#if defined ( _WIN32 ) && defined ( _MSC_VER )
#include <io.h>
#include <fcntl.h>
#else
#include <clocale>
#endif

namespace Lexer = MaryLang::Lexer;

int main( int argc, char **argv )
{
#if defined ( _WIN32 ) && defined ( _MSC_VER )
	_setmode( _fileno( stdout ), _O_U16TEXT );
#else
	std::setlocale( LC_ALL, "en_US.utf8" );
#endif

	if( argc < 2 ) return -1;

	Lexer::Scanner scanner( argv[1] );
	Lexer::Token token = scanner.GetNextToken();
	while( token.Type() != Lexer::TokenType::TK_EOF )
	{
		if( token.Type() != Lexer::TokenType::TK_INVALID )
			std::wcout << token << std::endl;
		
		token = scanner.GetNextToken();
	}
	return 0;
}