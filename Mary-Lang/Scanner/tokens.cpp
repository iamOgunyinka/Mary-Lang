#include "tokens.hpp"

namespace MaryLang
{
	namespace Lexer
	{
		Token::LookupTable Token::lookup_table;
		
		void Token::InitLookupTable()
		{
			lookup_table.insert( std::make_pair( L"var",	TokenType::TK_VAR ) );
			lookup_table.insert( std::make_pair( L"for",	TokenType::TK_FOR ) );
			lookup_table.insert( std::make_pair( L"do",		TokenType::TK_DO ) );
			lookup_table.insert( std::make_pair( L"while",	TokenType::TK_WHILE ) );
			lookup_table.insert( std::make_pair( L"if",		TokenType::TK_IF ) );
			lookup_table.insert( std::make_pair( L"isit",	TokenType::TK_ISIT ) );
			lookup_table.insert( std::make_pair( L"else",	TokenType::TK_ELSE ) );
			lookup_table.insert( std::make_pair( L"function", TokenType::TK_FUNCTION ) );
			lookup_table.insert( std::make_pair( L"among",		TokenType::TK_AMONG ) );
			lookup_table.insert( std::make_pair( L"check",	TokenType::TK_CHECK ) );
			lookup_table.insert( std::make_pair( L"continue", TokenType::TK_CONTINUE ) );
			lookup_table.insert( std::make_pair( L"leave",	TokenType::TK_LEAVE ) );
			lookup_table.insert( std::make_pair( L"int",	TokenType::TK_INT ) );
			lookup_table.insert( std::make_pair( L"double", TokenType::TK_DOUBLE ) );
			lookup_table.insert( std::make_pair( L"decltype", TokenType::TK_DECLTYPE ) );
			lookup_table.insert( std::make_pair( L"string", TokenType::TK_STRING ) );
			lookup_table.insert( std::make_pair( L"boolean", TokenType::TK_BOOLEAN ) );
			lookup_table.insert( std::make_pair( L"true",	TokenType::TK_TRUE ) );
			lookup_table.insert( std::make_pair( L"false",	TokenType::TK_FALSE ) );
			lookup_table.insert( std::make_pair( L"typeof",	TokenType::TK_TYPEOF ) );
			lookup_table.insert( std::make_pair( L"enum",	TokenType::TK_ENUM ) );
			lookup_table.insert( std::make_pair( L"return", TokenType::TK_RETURN ) );
			lookup_table.insert( std::make_pair( L"class",	TokenType::TK_CLASS ) );
			lookup_table.insert( std::make_pair( L"extends", TokenType::TK_EXTENDS ) );
			lookup_table.insert( std::make_pair( L"construct", TokenType::TK_CONSTRUCT ) );
		} // Token::initLookupTable

		wchar_t const* Token::GetName( TokenType tt ) const
		{
			switch( tt )
			{
			case TokenType::TK_ADD:			return L"+";
			case TokenType::TK_ADDEQL:		return L"+=";
			case TokenType::TK_AND:			return L"&";
			case TokenType::TK_ANDEQL:		return L"&=";
			case TokenType::TK_ASSIGN:		return L"=";
			case TokenType::TK_AT:  		return L"@";
			case TokenType::TK_COLON:		return L":";
			case TokenType::TK_COMMA:		return L",";
			case TokenType::TK_DECREMENT:	return L"--";
			case TokenType::TK_DIV:			return L"/";
			case TokenType::TK_DIVEQL:		return L"/=";
			case TokenType::TK_DOT:			return L".";
			case TokenType::TK_EQL:			return L"==";
			case TokenType::TK_GEQL:		return L">=";
			case TokenType::TK_GREATER:		return L">";
			case TokenType::TK_INCREMENT:	return L"++";
			case TokenType::TK_LAND:		return L"&&";
			case TokenType::TK_LBRACE:		return L"{";
			case TokenType::TK_LBRACKET:	return L"[";
			case TokenType::TK_LEQL:		return L"<=";
			case TokenType::TK_LESS:		return L"<";
			case TokenType::TK_LOR:			return L"||";
			case TokenType::TK_LPAREN:		return L"(";
			case TokenType::TK_LSASSIGN:	return L"<<=";
			case TokenType::TK_LSHIFT:		return L"<<";
			case TokenType::TK_MODASSIGN:	return L"%=";
			case TokenType::TK_MODULO:		return L"%";
			case TokenType::TK_MUL:			return L"*";
			case TokenType::TK_MULEQL:		return L"*=";
			case TokenType::TK_NEG:			return L"~";
			case TokenType::TK_NOT:			return L"!";
			case TokenType::TK_NOTEQL:		return L"!=";
			case TokenType::TK_OR:			return L"|";
			case TokenType::TK_OREQL:		return L"|=";
			case TokenType::TK_QMARK:		return L"?";
			case TokenType::TK_RBRACE:		return L"}";
			case TokenType::TK_RBRACKET:	return L"]";
			case TokenType::TK_RPAREN:		return L")";
			case TokenType::TK_RSASSIGN:	return L">>=";
			case TokenType::TK_RSHIFT:		return L">>";
			case TokenType::TK_SEMICOLON:	return L";";
			case TokenType::TK_SUB:			return L"-";
			case TokenType::TK_SUBEQL:		return L"-=";
			case TokenType::TK_XOR:     	return L"^";
			case TokenType::TK_XORASSIGN:	return L"^=";
			default:						return L"Invalid";
			}
		}
	} // namespace Lexer
} // namespace MaryLang
