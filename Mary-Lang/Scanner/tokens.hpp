#pragma once

#include <memory>
#include <unordered_map>
#include "../Utils/Position.hpp"
#include "../Utils/Utils.hpp"

namespace Mary
{
	namespace Support
	{
		struct WStrHash 
		{
			std::size_t operator()( wchar_t const * const wstr ) const
			{
				wchar_t const *i = wstr;
				int offset = L'a' - 1;
				std::size_t hash = 0;

				while( *i != L'\0' ){
					hash = hash << 1 ^ ( *i - offset );
					++i;
				}
				return hash;
			}
		};
		// To-DO: Verify its correctness
		struct WStrEqual
		{
			bool operator ()( wchar_t const * const previous, 
				wchar_t const * const current ) const
			{
				return std::wcscmp( previous, current ) == 0;
			}
		};
	} //namespace Support
	namespace Lexer
	{
		enum class TokenType
		{
			TK_EOF,
			TK_INVALID,

			TK_IDENTIFIER,
			TK_STRLITERAL,
			TK_STRLITINTERPOL, /* string interpolation like "I am #{name}" */
			TK_CONSTANT,

			/* Keywords */
			TK_VAR,
			TK_FOR,
			TK_DO,
			TK_WHILE,
			TK_IF,
			TK_ELSE,
			TK_AMONG,
			TK_CHECK,
			TK_CONTINUE,
			TK_LEAVE,
			TK_INT,
			TK_DOUBLE,
			TK_STRING,
			TK_BOOLEAN,
			TK_TRUE,
			TK_FALSE,
			TK_TYPEOF,
			TK_ENUM,
			TK_RETURN,
			TK_STATIC,
			TK_CLASS,
			TK_EXTENDS,
			TK_CONSTRUCT,
			TK_DECLTYPE,

			/* Operators */
			TK_AT,
			TK_DOT,
			TK_INCREMENT, // ++
			TK_DECREMENT, // --
			TK_AND, // &
			TK_ANDEQL, // &=
			TK_LAND, // Logical &&
			TK_OR, // |
			TK_LOR, // Logical ||
			TK_OREQL, // |=
			TK_XOR, // ^
			TK_XORASSIGN, // ^=
			TK_NEG, // ~
			TK_NOT, // !
			TK_MODULO, // %
			TK_MODASSIGN, // %=

			TK_LPAREN, // (
			TK_RPAREN, // )
			TK_LBRACKET, // [ 
			TK_RBRACKET, // ]
			TK_LBRACE, // {
			TK_RBRACE, // }
			TK_ADD,    // +
			TK_SUB,    // -
			TK_MUL,    // *
			TK_DIV,    // /
			TK_ADDEQL, // +=
			TK_SUBEQL, // -=
			TK_MULEQL, // *=
			TK_DIVEQL, // /=
			TK_LESS,   // <
			TK_GREATER, // >
			TK_LEQL,   // <=
			TK_LSASSIGN, // <<= 
			TK_GEQL,   // >=
			TK_RSASSIGN, // >>=
			TK_EQL,    // ==
			TK_NOTEQL,   // !=
			TK_LSHIFT, // <<
			TK_RSHIFT, // >>
			TK_COLON, // :
			TK_SEMICOLON, // ;
			TK_COMMA, // ,
			TK_QMARK, // ?
			TK_ASSIGN // =
		}; // enum TokenType

		struct Token 
		{
			Token( wchar_t *id, Support::Position const & pos, TokenType type )
				:_id( id ),
				_pos( pos ),
				_type( type )
			{
			}

			Token( Support::Position pos, TokenType type )
				: _id( nullptr ),
				_pos( std::move( pos ) ),
				_type( type )
			{
				auto c = getName( type );
				_id = Mary::Support::mystrndup( c, wcslen( c ) );
			}

			Token& operator=( Token const & t )
			{
				if( this != &t )
				{
					_pos = t.pos();
					_type = t.type();
					delete [] _id;
					_id = Mary::Support::mystrndup( t.id(), wcslen( t.id() ) );
				}
				return *this;
			}

			~Token(){ delete[] _id; }

			wchar_t const*      id() const { return _id; }
			Support::Position	pos() const { return _pos; }
			TokenType			type() const { return _type; }

			typedef std::unordered_map< wchar_t const *, TokenType, Mary::Support::WStrHash, 
				Mary::Support::WStrEqual> LookupTable;

			static LookupTable lookup_table;
			static void initLookupTable();

			friend std::wostream & operator<<( std::wostream & os, Token const & t ){
				return os << L"ID: '" << t.id() << L"', " << t.pos() << L" : " 
					<< static_cast<int>( t.type() );
			}
		private:
			wchar_t const *				getName( TokenType tt ) const;
			wchar_t					    *_id;
			Support::Position			_pos;
			TokenType					_type;
		}; // struct Token
	} // namespace Lexer
} //namespace Mary
