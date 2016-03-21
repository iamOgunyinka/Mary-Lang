#include "Scanner.hpp"
#include <fstream>
#include <cwctype>
#include "../Utils/Utils.hpp"

#if defined( _WIN32 )
#include <Windows.h>
#else
#include <cstdlib>
#include <clocale>
#endif

#define isHexNumber(c) ( ( c >= L'A' && c <= L'F' ) || ( c >= L'a' && c <= L'f' ) || ( c >= L'0' && c <= L'9' ))
#define isOctalNumber(c) ( c >= L'0' && c <= L'7' )

namespace MaryLang
{
	namespace Lexer
	{
		Scanner::Scanner( char const * filename )
			:diag( true ), pos( 0, 0 ), 
			buffer( nullptr ), current_token( '\n' ),
			marker_position( 0 ), begin_mark( 0 ),
			buffer_size( 0 )
		{
			if( SetNewFileName( filename ) ){
				Token::InitLookupTable();
			} else {
				exit( -1 );
			}
		}

		Scanner::~Scanner()
		{
			delete []buffer;
		}

		void Scanner::NextChar()
		{
			if( marker_position == buffer_size ) {
				current_token = L'\0';

				return;
			}

			if( current_token == L'\n' ){
				++pos._line_number;
				pos._column_number = 1;
			} else {
				++pos._column_number;
			}
			current_token = buffer[marker_position];
			++marker_position;
		}

		bool Scanner::SetNewFileName( char const * filename )
		{
			char* buffer;
			std::ifstream in( filename, std::ios_base::in | std::ios_base::binary 
				| std::ios_base::ate );
			if( in ) {
				// get file size
				in.seekg(0, std::ios::end);
				buffer_size = (size_t)in.tellg();
				in.seekg(0, std::ios::beg);
				buffer = (char*)calloc(buffer_size + 1, sizeof(char));
				in.read(buffer, buffer_size);
				// close file
				in.close();
			}
			else {
				std::wcerr << L"Unable to open source file: " << filename << std::endl;
				return false;
			}

			// convert unicode
#if defined( _WIN32 ) && defined( _MSC_VER )
			int wsize = MultiByteToWideChar( CP_UTF8, 0, buffer, -1, nullptr, 0);
			if( !wsize ) {
				std::wcerr << L"Unable to open source file: " << filename << std::endl;
				return false;
			}
			wchar_t* wbuffer = new wchar_t[wsize];
			int check = MultiByteToWideChar( CP_UTF8, 0, buffer, -1, wbuffer, wsize);
			if(!check) {
				std::wcerr << L"Unable to open source file: " << filename << std::endl;
				return false;
			}
#else
			size_t wsize = std::mbstowcs( nullptr, buffer, buffer_size);
			if( wsize == (size_t)-1 ) {
				free( buffer );
				std::wcerr << L"Unable to open source file: " << filename << std::endl;
				return false;
			}
			wchar_t* wbuffer = new wchar_t[wsize + 1];
			size_t check = std::mbstowcs( wbuffer, buffer, buffer_size );
			if( check == (size_t)-1 ) {
				free( buffer );
				delete[] wbuffer;
				std::wcerr << L"Unable to open source file: " << filename << std::endl;
				return false;
			}
			wbuffer[wsize] = L'\0';
#endif
			free(buffer);
			this->buffer = wbuffer;
			return true;
		}

		Token Scanner::GetNextToken()
		{
			for( ; ; )
			{
				if( EOF != current_token 
					&& ( std::iswalpha( current_token ) || current_token == L'_' ) )
				{
					return IdentifierOrKeywordToken();
				} else {
					switch ( current_token )
					{
					case L'\0':
						marker_position = buffer_size;
						return Token( pos, TokenType::TK_EOF );
					case L' ':
					case L'\r':
					case L'\t':
					case L'\n':
					case L'\v':
					case L'\f':
						NextChar();
						continue;

					case L'0':
					case L'1':
					case L'2':
					case L'3':
					case L'4':
					case L'5':
					case L'6':
					case L'7':
					case L'8':
					case L'9':
						return GetNumberToken();

					case L'"':
					case L'\'':
						return GetStringLiteralToken();
					case L'.':
						NextChar();
						return Token( pos, TokenType::TK_DOT );
					case L'+':
						{
							Support::Position newPos( pos );
							NextChar();
							switch( current_token )
							{
							case L'+': NextChar(); return Token( newPos, TokenType::TK_INCREMENT );
							case L'=': NextChar(); return Token( newPos, TokenType::TK_ADDEQL );
							default: return Token( newPos, TokenType::TK_ADD );
							}
						}
					case L'-':
						{
							Support::Position newPos( pos );
							NextChar();
							switch( current_token )
							{
							case L'-': NextChar(); return Token( newPos, TokenType::TK_DECREMENT );
							case L'=': NextChar(); return Token( newPos, TokenType::TK_SUBEQL );
							default: return Token( newPos, TokenType::TK_SUB );
							}
						}
					case L'*':
						{
							Support::Position newPos( pos );
							NextChar();
							switch( current_token )
							{
							case L'=': NextChar(); return Token( newPos, TokenType::TK_MULEQL );
							default: return Token( newPos, TokenType::TK_MUL );
							}
						}
					case L'/':
						{
							Support::Position newPos( pos );
							NextChar();
							switch( current_token )
							{
							case L'=':
								NextChar();
								return Token( newPos, TokenType::TK_DIVEQL );
							case L'*': 
								{
									NextChar();
									while( current_token != EOF && 
										!( current_token == L'*' && buffer[ marker_position ] == L'/' )){
											NextChar();
									}
									if( current_token == EOF ) {
										throw std::runtime_error( "Unterminated comment" );
									}
									NextChar();
									NextChar();
									continue;
								}
							case L'/':
								while( current_token != L'\n' ) NextChar();
								continue;
							default:
								return Token( newPos, TokenType::TK_DIV );
							}
						}
					case L'&':
						NextChar();
						switch( current_token ){
						case L'&': NextChar(); return Token( pos, TokenType::TK_LAND );
						case L'=': NextChar(); return Token( pos, TokenType::TK_ANDEQL );
						default: return Token( pos, TokenType::TK_AND );
						}
					case L'|':
						NextChar();
						switch( current_token ){
						case L'|': NextChar(); return Token( pos, TokenType::TK_LOR );
						case L'=': NextChar(); return Token( pos, TokenType::TK_OREQL );
						default: return Token( pos, TokenType::TK_OR );
						}
					case L'^': 
						NextChar();
						switch( current_token )
						{
						case L'=': NextChar(); return Token( pos, TokenType::TK_XORASSIGN );
						default:   return Token( pos, TokenType::TK_XOR );
						}
					case L'~': NextChar(); return Token( pos, TokenType::TK_NEG );
					case L'!':
						NextChar();
						switch( current_token ){
						case L'=': NextChar(); return Token( pos, TokenType::TK_NOTEQL );
						default: return Token( pos, TokenType::TK_NOT );
						}
					case L'<':
						NextChar();
						switch ( current_token )
						{
						case L'<':
							NextChar(); 
							switch( current_token ){
							case L'=': NextChar(); return Token( pos, TokenType::TK_LSASSIGN );
							default: return Token( pos, TokenType::TK_LSHIFT );
							}
						case L'=': NextChar(); return Token( pos, TokenType::TK_LEQL );
						default: return Token( pos, TokenType::TK_LESS );
						}
					case L'>':
						NextChar();
						switch ( current_token )
						{
						case L'>': 
							NextChar(); 
							switch( current_token )
							{
							case L'=': NextChar(); return Token( pos, TokenType::TK_RSASSIGN );
							default: return Token( pos, TokenType::TK_RSHIFT );
							}
						case L'=': NextChar(); return Token( pos, TokenType::TK_GEQL );
						default: return Token( pos, TokenType::TK_GREATER );
						}
					case L'%':
						NextChar();
						switch ( current_token )
						{
						case L'=': NextChar(); return Token( pos, TokenType::TK_MODASSIGN );
						default: return Token( pos, TokenType::TK_MODULO );
						}
					case L'(':
						NextChar(); return Token( pos, TokenType::TK_LPAREN );
					case L')':
						NextChar(); return Token( pos, TokenType::TK_RPAREN );
					case L'[':
						NextChar(); return Token( pos, TokenType::TK_LBRACKET );
					case L']':
						NextChar(); return Token( pos, TokenType::TK_RBRACKET );
					case L'{':
						NextChar(); return Token( pos, TokenType::TK_LBRACE );
					case L'}':
						NextChar(); return Token( pos, TokenType::TK_RBRACE );
					case L'=':
						NextChar();
						switch ( current_token )
						{
						case L'=':
							NextChar();
							return Token( pos, TokenType::TK_EQL );
						default: return Token( pos, TokenType::TK_ASSIGN );
						}
					case L'@':
						NextChar();
						return Token( pos, TokenType::TK_AT );
					case L':':
						NextChar();
						return Token( pos, TokenType::TK_COLON );
					case L';':
						NextChar();
						return Token( pos, TokenType::TK_SEMICOLON );
					case L',':
						NextChar();
						return Token( pos, TokenType::TK_COMMA );
					default:
						NextChar();
						diag.Warning( pos, L"Invalid character" );
						return Token( pos, TokenType::TK_INVALID );
					}
				}
			}
		} // Scanner::getNextToken()

		Token Scanner::IdentifierOrKeywordToken()
		{
			begin_mark = marker_position - 1;
			Support::Position newPos = pos;
			NextChar();
			while( ( std::iswalnum( current_token ) || current_token == L'_' ) 
				&& !( current_token == EOF ) )
			{ 
				NextChar();
			}

			if( current_token == EOF ){
				return Token( newPos, TokenType::TK_EOF );
			}
			return IdentifierOrKeywordToken( newPos );
		} // Scanner::identifierOrKeyword

		Token Scanner::IdentifierOrKeywordToken( Support::Position pos )
		{
			auto wsize = marker_position - begin_mark;
			wchar_t *tk = Support::Mystrndup( &buffer[begin_mark], wsize - 1 );
			if( tk == nullptr ){
				std::wcerr << "Allocation/Copy failed" << std::endl;
				exit( 1 );
			}
			auto f = Token::lookup_table.find( tk );
			TokenType type = ( f == Token::lookup_table.end() ) ? TokenType::TK_IDENTIFIER : f->second;
			return Token( tk, pos, type );
		}

		Token Scanner::GetIntegerToken()
		{
			// gather as many digits as we can get. Are we done?
			bool isDecimal = false;
			do {
				NextChar();
			} while( std::iswdigit( current_token ) );

			// OK! Did we hit a decimal point?
			if( current_token == L'.' ){
				isDecimal = true;
				NextChar();
			} else if( current_token == L'E' || current_token == L'e' ) {
				// Or perhaps an exponential sign?
				NextChar();
				// with a positiive or negative sign?
				if( ( current_token == L'+' || current_token == L'-' )
					&& std::iswdigit( buffer[marker_position] ) )
				{
					NextChar();
				}
			}
			//lets gather the rest of the circle member and return home.
			while( std::iswdigit( current_token )){
				NextChar();
			}

			return Token( Support::Mystrndup( 
				&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ),
				pos, ( isDecimal ? TokenType::TK_DOUBLE : TokenType::TK_INT ) );
		}

		Token Scanner::GetNumberToken()
		{
			Support::Position newPos = pos;
			begin_mark = marker_position - 1;
			wchar_t next_char_lookahead = buffer[marker_position];
			if( current_token == L'0' && 
				( next_char_lookahead == L'x' || next_char_lookahead == L'X' ) )
			{
				NextChar();
				do {
					NextChar();
				} while( current_token != EOF && isHexNumber( current_token ));

				if( marker_position == EOF ){
					diag.Warning( newPos, L"End of file encountered" );
				}
				return Token( Support::Mystrndup(
					&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ), 
					newPos, TokenType::TK_INT );
			} else if( current_token == L'0' && isOctalNumber( next_char_lookahead ) ) {
				do {
					NextChar();
				} while( current_token != EOF && isOctalNumber( current_token ) );
				return Token( Support::Mystrndup( 
					&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ), 
					newPos, TokenType::TK_INT );
			} else if( current_token == L'0' && 
				( next_char_lookahead == L'b' || next_char_lookahead == L'B' )) 
			{
				NextChar();
				do {
					NextChar();
				} while( current_token == L'0' || current_token == L'1' );

				if( std::iswdigit( current_token ) ){
					diag.Warning( pos, L"Invalid binary digit" );
					while( std::iswdigit( current_token ) ) NextChar();
				}
				return Token( Support::Mystrndup( 
					&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ),
					newPos, TokenType::TK_INT );
			}

			return GetIntegerToken();
		} // Scanner::getNumber()

		// TO-DO: Recongize string interpolation.
		Token Scanner::GetStringLiteralToken()
		{
			Support::Position newPos ( pos );
			std::wstring buf;
			int const delimeter = current_token;
			NextChar();

			bool hasError = false, strInterpolOpen = false, strInterpolAvailable = false;
			for( ; ; ){
				if( current_token == EOF || current_token == L'\n' ){
					diag.Error( pos, L"Expected a delimeter in string." );
					return Token( pos, TokenType::TK_INVALID );
					marker_position = buffer_size;
				}

				if( current_token == delimeter ){
					NextChar();
					break;
				} else if( current_token == L'\\' ) {
					wchar_t peek = buffer[marker_position];
					if( peek == delimeter ){
						NextChar();
					}
				} else if( current_token == L'#' ){
					buf.push_back( current_token );
					NextChar();
					if( current_token == L'{' ){ 
						buf.push_back( current_token );
						strInterpolOpen = true;
						NextChar();
					}
					continue;
				} else if( current_token == L'}' && strInterpolOpen ){
					strInterpolOpen = false;
					strInterpolAvailable = true;
				}
				buf.push_back( current_token );
				NextChar();
			}
			return Token( Support::Mystrndup( buf.c_str(), buf.size() ),
				newPos, 
				( strInterpolAvailable ? TokenType::TK_STRLITINTERPOL : TokenType::TK_STRLITERAL ) );
		}
	} // namespace Lexer
} // namespace MaryLang
