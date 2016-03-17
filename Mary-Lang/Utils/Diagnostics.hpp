#pragma once

#include <cstdarg>
#include <cstdio>
#include "Position.hpp"

#define RESET       "\033[0m"
#define BOLD        "\033[1;37m"
#define NOTE        "\033[1;2;37m"
#define WARNING     "\033[1;35m"
#define ERROR       "\033[1;31m"

namespace Mary
{
	namespace Support
	{
		struct Diagnostic 
		{
			Diagnostic( const bool color ): color_(color), numErrors_( 0 )
			{ }

			inline void note( Position const & pos, wchar_t const * note ) {
				print_pos( pos, Kind::K_Note, note );
			}

			inline void warning( Position const & pos, wchar_t const * w ) {
				print_pos( pos, Kind::K_Warning, w );
			}

			inline void error( wchar_t const * what )
			{
				if ( color_ ) std::cerr << BOLD;
				std::cerr << what << std::endl;
				if ( color_ ) std::cerr << RESET;
			}
			inline void error( Position const & pos, wchar_t const * m ) {
				++numErrors_;
				print_pos( pos, Kind::K_Error, m );
			}

			unsigned hasError() {
				auto tmp = numErrors_;
				numErrors_ = 0;
				return tmp;
			}

		private:
			const bool color_;
			unsigned int numErrors_;

			enum class Kind {
				K_Note,
				K_Warning,
				K_Error
			};

			void print_pos( const Position pos, const Kind kind, wchar_t const * what ) {
				switch ( kind ) {
				case Kind::K_Note:
					if (color_) { std::cerr << NOTE;    } 
					std::wcerr << L"note: ";
					break;
				case Kind::K_Warning: 
					if (color_) { std::cerr << WARNING; } 
					std::wcerr << L"warning: "; 
					break;
				case Kind::K_Error:   
					if (color_) { std::cerr << ERROR;   } 
					std::wcerr << L"error: ";
					break;
				}
				if ( color_) std::cerr << BOLD;
				std::wcerr << what << L" on ";
				pos.dump();
				if (color_ ) std::cerr << RESET;
				
				if (color_) std::cerr << RESET;
				std::endl( std::cerr );
			}
		};
		static void ReportError( wchar_t const * what )
		{
			static Diagnostic diag = true;
			diag.error( what );
		}
#undef RESET
#undef BOLD
#undef NOTE
#undef WARNING
#undef ERROR
	} // namespace Support
} // namespace Mary
