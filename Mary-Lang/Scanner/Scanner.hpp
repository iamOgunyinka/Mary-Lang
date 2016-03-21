#pragma once
#include "Tokens.hpp"
#include "../Utils/Diagnostics.hpp"
#include <memory>

namespace MaryLang
{
	namespace Lexer
	{
		struct Scanner
		{
		private:
			Support::Diagnostic	diag;
			Support::Position	pos;
			wchar_t				*buffer;
			wchar_t				current_token;
			int					marker_position, begin_mark;
			int					buffer_size;
		private:
			Token	GetNumberToken();
			Token	GetIntegerToken();
			Token	GetStringLiteralToken();
			void	NextChar();
			Token	IdentifierOrKeywordToken();
			Token	IdentifierOrKeywordToken( Support::Position pos );
		public:
			Scanner( char const * filename );
			~Scanner();
			Token	GetNextToken();
			bool	SetNewFileName( char const * filename );
		}; // Scanner
	}
} // namespace MaryLang