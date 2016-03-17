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

namespace Mary
{
	namespace Lexer
	{
		Scanner::Scanner( char const * filename )
			:diag( true ), pos( 0, 0 ), 
			buffer( nullptr ), current_token( '\n' ),
			marker_position( 0 ), begin_mark( 0 ),
			buffer_size( 0 )
		{
			std::ifstream file( filename );
			char *temp_buffer = nullptr;
			if( file ) {
				file.seekg( 0, file.end );
				buffer_size = static_cast<std::size_t>( file.tellg() );
				temp_buffer = new char[ buffer_size + 1];
				file.seekg( file.beg );
				file.read( temp_buffer, buffer_size );
			} else {
				std::wcerr << L"Unable to open specified file" << std::endl;
				exit( 1 );
			}
#ifdef _WIN32
			auto wsize = MultiByteToWideChar( CP_UTF8, 0, temp_buffer, -1, NULL, 0 );
			if( wsize == 0 ){
				delete []temp_buffer;
				std::wcerr << L"Unable to convert file to Unicode" << std::endl;
				exit( 1 );
			} else {
				buffer = new wchar_t[ wsize + 1 ];
				int new_wsize = MultiByteToWideChar( CP_UTF8, 0, temp_buffer, -1, buffer, wsize );
				if( new_wsize == 0 ){
					delete []temp_buffer;
					std::wcerr << L"Unable to convert file characters to Unicode" << std::endl;
					exit( 1 );
				}
			}
#else
			std::size_t wsize = std::mbstowcs( NULL, temp_buffer, buffer_size );
			if( wsize == static_cast<std::size_t>( -1 ) ){
				delete []temp_buffer;
				std::wcerr << L"Unable to convert file character to Unicode" << std::endl;
				exit( 1 );
			} else {
				buffer = new wchar_t[ wsize ];
				std::size_t new_wsize = std::mbstowcs( buffer, temp_buffer, buffer_size );
				if( new_wsize == static_cast<std::size_t>( -1 ) ){
					delete []temp_buffer;
					std::wcerr << L"Unable to convert file character to Unicode" << std::endl;
					exit( 1 );
				}
			}
#endif
			if( temp_buffer ) delete []temp_buffer;
			Token::initLookupTable();
		}

		Scanner::~Scanner()
		{
			delete []buffer;
		}

		void Scanner::nextChar()
		{
			if( marker_position == buffer_size ) {
				current_token = EOF;
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

		Token Scanner::getNextToken()
		{
			for( ; ; )
			{
				if( EOF != current_token 
					&& ( std::iswalpha( current_token ) || current_token == L'_' ) )
				{
					return identifierOrKeywordToken();
				} else {
					switch ( current_token )
					{
					case EOF:
						marker_position = buffer_size;
						return Token( pos, TokenType::TK_EOF );
					case L' ':
					case L'\r':
					case L'\t':
					case L'\n':
					case L'\v':
					case L'\f':
						nextChar();
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
						return getNumberToken();

					case L'"':
					case L'\'':
						return getStringLiteralToken();
					case L'.':
						nextChar();
						return Token( pos, TokenType::TK_DOT );
					case L'+':
						{
							Support::Position newPos( pos );
							nextChar();
							switch( current_token )
							{
							case L'+': nextChar(); return Token( newPos, TokenType::TK_INCREMENT );
							case L'=': nextChar(); return Token( newPos, TokenType::TK_ADDEQL );
							default: return Token( newPos, TokenType::TK_ADD );
							}
						}
					case L'-':
						{
							Support::Position newPos( pos );
							nextChar();
							switch( current_token )
							{
							case L'-': nextChar(); return Token( newPos, TokenType::TK_DECREMENT );
							case L'=': nextChar(); return Token( newPos, TokenType::TK_SUBEQL );
							default: return Token( newPos, TokenType::TK_SUB );
							}
						}
					case L'*':
						{
							Support::Position newPos( pos );
							nextChar();
							switch( current_token )
							{
							case L'=': nextChar(); return Token( newPos, TokenType::TK_MULEQL );
							default: return Token( newPos, TokenType::TK_MUL );
							}
						}
					case L'/':
						{
							Support::Position newPos( pos );
							nextChar();
							switch( current_token )
							{
							case L'=':
								nextChar();
								return Token( newPos, TokenType::TK_DIVEQL );
							case L'*': 
								{
									nextChar();
									while( current_token != EOF && 
										!( current_token == L'*' && buffer[ marker_position ] == L'/' )){
											nextChar();
									}
									if( current_token == EOF ) {
										throw std::runtime_error( "Unterminated comment" );
									}
									nextChar();
									nextChar();
									continue;
								}
							case L'/':
								while( current_token != L'\n' ) nextChar();
								continue;
							default:
								return Token( newPos, TokenType::TK_DIV );
							}
						}
					case L'&':
						nextChar();
						switch( current_token ){
						case L'&': nextChar(); return Token( pos, TokenType::TK_LAND );
						case L'=': nextChar(); return Token( pos, TokenType::TK_ANDEQL );
						default: return Token( pos, TokenType::TK_AND );
						}
					case L'|':
						nextChar();
						switch( current_token ){
						case L'|': nextChar(); return Token( pos, TokenType::TK_LOR );
						case L'=': nextChar(); return Token( pos, TokenType::TK_OREQL );
						default: return Token( pos, TokenType::TK_OR );
						}
					case L'^': 
						nextChar();
						switch( current_token )
						{
						case L'=': nextChar(); return Token( pos, TokenType::TK_XORASSIGN );
						default:   return Token( pos, TokenType::TK_XOR );
						}
					case L'~': nextChar(); return Token( pos, TokenType::TK_NEG );
					case L'!':
						nextChar();
						switch( current_token ){
						case L'=': nextChar(); return Token( pos, TokenType::TK_NOTEQL );
						default: return Token( pos, TokenType::TK_NOT );
						}
					case L'<':
						nextChar();
						switch ( current_token )
						{
						case L'<':
                            nextChar(); 
                            switch( current_token ){
							case L'=': nextChar(); return Token( pos, TokenType::TK_LSASSIGN );
                            default: return Token( pos, TokenType::TK_LSHIFT );
                            }
						case L'=': nextChar(); return Token( pos, TokenType::TK_LEQL );
						default: return Token( pos, TokenType::TK_LESS );
						}
					case L'>':
						nextChar();
						switch ( current_token )
						{
						case L'>': 
                            nextChar(); 
                            switch( current_token )
                            {
                            case L'=': nextChar(); return Token( pos, TokenType::TK_RSASSIGN );
                            default: return Token( pos, TokenType::TK_RSHIFT );
                            }
						case L'=': nextChar(); return Token( pos, TokenType::TK_GEQL );
						default: return Token( pos, TokenType::TK_GREATER );
						}
					case L'%':
						nextChar();
						switch ( current_token )
						{
						case L'=': nextChar(); return Token( pos, TokenType::TK_MODASSIGN );
						default: return Token( pos, TokenType::TK_MODULO );
						}
					case L'(':
						nextChar(); return Token( pos, TokenType::TK_LPAREN );
					case L')':
						nextChar(); return Token( pos, TokenType::TK_RPAREN );
					case L'[':
						nextChar(); return Token( pos, TokenType::TK_LBRACKET );
					case L']':
						nextChar(); return Token( pos, TokenType::TK_RBRACKET );
					case L'{':
						nextChar(); return Token( pos, TokenType::TK_LBRACE );
					case L'}':
						nextChar(); return Token( pos, TokenType::TK_RBRACE );
					case L'=':
						nextChar();
						switch ( current_token )
						{
						case L'=':
							nextChar();
							return Token( pos, TokenType::TK_EQL );
						default: return Token( pos, TokenType::TK_ASSIGN );
						}
                    case L'@':
                        nextChar();
                        return Token( pos, TokenType::TK_AT );
					case L':':
						nextChar();
						return Token( pos, TokenType::TK_COLON );
					case L';':
						nextChar();
						return Token( pos, TokenType::TK_SEMICOLON );
					case L',':
						nextChar();
						return Token( pos, TokenType::TK_COMMA );
					default:
                        //~nextChar();
						return Token( pos, TokenType::TK_INVALID );
					}
				}
			}
		} // Scanner::getNextToken()

		Token Scanner::identifierOrKeywordToken()
		{
			begin_mark = marker_position - 1;
			Support::Position newPos = pos;
			nextChar();
			while( ( std::iswalnum( current_token ) || current_token == L'_' ) 
				&& !( current_token == EOF ) )
			{ 
				nextChar();
			}

			if( current_token == EOF ){
				return Token( newPos, TokenType::TK_EOF );
			}
			return identifierOrKeywordToken( newPos );
		} // Scanner::identifierOrKeyword

		Token Scanner::identifierOrKeywordToken( Support::Position pos )
		{
			auto wsize = marker_position - begin_mark;
			wchar_t *tk = Support::mystrndup( &buffer[begin_mark], wsize - 1 );
			if( tk == nullptr ){
				std::wcerr << "Allocation/Copy failed" << std::endl;
				exit( 1 );
			}
			auto f = Token::lookup_table.find( tk );
			TokenType type = ( f == Token::lookup_table.end() ) ? TokenType::TK_IDENTIFIER : f->second;
			return Token( tk, pos, type );
		}

		Token Scanner::getIntegerToken()
		{
			// gather as many digits as we can get. Are we done?
			bool isDecimal = false;
			do {
				nextChar();
			} while( std::iswdigit( current_token ) );

			// OK! Did we hit a decimal point?
			if( current_token == L'.' ){
				isDecimal = true;
				nextChar();
			} else if( current_token == L'E' || current_token == L'e' ) {
				// Or perhaps an exponential sign?
				nextChar();
				// with a positiive or negative sign?
				if( ( current_token == L'+' || current_token == L'-' )
					&& std::iswdigit( buffer[marker_position] ) )
				{
					nextChar();
				}
			}
			//lets gather the rest of the circle member and return home.
			while( std::iswdigit( current_token )){
				nextChar();
			}

			return Token( Support::mystrndup( 
				&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ),
				pos, ( isDecimal ? TokenType::TK_DOUBLE : TokenType::TK_INT ) );
		}

		Token Scanner::getNumberToken()
		{
			Support::Position newPos = pos;
			begin_mark = marker_position - 1;
			wchar_t next_char_lookahead = buffer[marker_position];
			if( current_token == L'0' && 
				( next_char_lookahead == L'x' || next_char_lookahead == L'X' ) )
			{
				nextChar();
				do {
					nextChar();
				} while( current_token != EOF && isHexNumber( current_token ));

				if( marker_position == EOF ){
					diag.warning( newPos, L"End of file encountered" );
				}
				return Token( Support::mystrndup(
					&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ), 
					newPos, TokenType::TK_INT );
			} else if( current_token == L'0' && isOctalNumber( next_char_lookahead ) ) {
				do {
					nextChar();
				} while( current_token != EOF && isOctalNumber( current_token ) );
				return Token( Support::mystrndup( 
					&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ), 
					newPos, TokenType::TK_INT );
			} else if( current_token == L'0' && 
				( next_char_lookahead == L'b' || next_char_lookahead == L'B' )) 
			{
				nextChar();
				do {
					nextChar();
				} while( current_token == L'0' || current_token == L'1' );

				if( std::iswdigit( current_token ) ){
					diag.warning( pos, L"Invalid binary digit" );
					while( std::iswdigit( current_token ) ) nextChar();
				}
				return Token( Support::mystrndup( 
					&buffer[begin_mark], ( marker_position - begin_mark ) - 1 ),
					newPos, TokenType::TK_INT );
			}

			return getIntegerToken();
		} // Scanner::getNumber()

		// TO-DO: Recongize string interpolation.
		Token Scanner::getStringLiteralToken()
		{
			Support::Position newPos ( pos );
			std::wstring buf;
			int const delimeter = current_token;
			nextChar();

			bool hasError = false, strInterpolOpen = false, strInterpolAvailable = false;
			for( ; ; ){
				if( current_token == EOF || current_token == L'\n' ){
					diag.error( pos, L"Expected a delimeter in string." );
					return Token( pos, TokenType::TK_INVALID );
					marker_position = buffer_size;
				}

				if( current_token == delimeter ){
					nextChar();
					break;
				} else if( current_token == L'\\' ) {
					wchar_t peek = buffer[marker_position];
					if( peek == delimeter ){
						nextChar();
					}
				} else if( current_token == L'#' ){
					buf.push_back( current_token );
					nextChar();
					if( current_token == L'{' ){ 
						buf.push_back( current_token );
						strInterpolOpen = true;
						nextChar();
					}
					continue;
				} else if( current_token == L'}' && strInterpolOpen ){
					strInterpolOpen = false;
					strInterpolAvailable = true;
				}
				buf.push_back( current_token );
				nextChar();
			}
			return Token( Support::mystrndup( buf.c_str(), buf.size() ),
				newPos, 
				( strInterpolAvailable ? TokenType::TK_STRLITINTERPOL : TokenType::TK_STRLITERAL ) );
		}
	} // namespace Lexer
} // namespace Mary
