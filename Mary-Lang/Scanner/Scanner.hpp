#pragma once
#include "Tokens.hpp"
#include "../Utils/Diagnostics.hpp"
#include <memory>

namespace Mary
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
			Token	getNumberToken();
			Token	getIntegerToken();
			Token	getStringLiteralToken();
			void	nextChar();
			Token	identifierOrKeywordToken();
			Token	identifierOrKeywordToken( Support::Position pos );
		public:
			Scanner( char const * filename );
			~Scanner();
			Token	getNextToken();
			bool	setNewFileName( char const * filename );
		}; // Scanner
	}
} // namespace Mary