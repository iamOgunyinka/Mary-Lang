#pragma once

#include <cstdarg>
#include <cstdio>
#include "Position.hpp"

#define DIAG_RESET       "\033[0m"
#define DIAG_BOLD        "\033[1;37m"
#define DIAG_NOTE        "\033[1;2;37m"
#define DIAG_WARNING     "\033[1;35m"
#define DIAG_ERROR       "\033[1;31m"

namespace MaryLang
{
	namespace Support
	{
		struct Diagnostic 
		{
			Diagnostic( const bool color ): color_(color), numErrors_( 0 )
			{ }

			inline void Note( Position const & pos, wchar_t const * note ) {
				PrintPos( pos, Kind::K_Note, note );
			}

			inline void Warning( Position const & pos, wchar_t const * w ) {
				PrintPos( pos, Kind::K_Warning, w );
			}

			inline void Error( wchar_t const * what )
			{
				if ( color_ ) std::cerr << DIAG_BOLD;
				std::cerr << what << std::endl;
				if ( color_ ) std::cerr << DIAG_RESET;
			}
			inline void Error( Position const & pos, wchar_t const * m ) {
				++numErrors_;
				PrintPos( pos, Kind::K_Error, m );
			}

			unsigned HasError() {
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

			void PrintPos( const Position pos, const Kind kind, wchar_t const * what ) {
				switch ( kind ) {
				case Kind::K_Note:
					if (color_) { std::cerr << DIAG_NOTE;    } 
					std::wcerr << L"note: ";
					break;
				case Kind::K_Warning: 
					if (color_) { std::cerr << DIAG_WARNING; } 
					std::wcerr << L"warning: "; 
					break;
				case Kind::K_Error:   
					if (color_) { std::cerr << DIAG_ERROR;   } 
					std::wcerr << L"error: ";
					break;
				}
				if ( color_) std::cerr << DIAG_BOLD;
				std::wcerr << what << L" on ";
				pos.Dump();
				if (color_ ) std::cerr << DIAG_RESET;
				
				if (color_) std::cerr << DIAG_RESET;
				std::endl( std::cerr );
			}
		};
		static void ReportError( wchar_t const * what )
		{
			static Diagnostic diag = true;
			diag.Error( what );
		}
#undef DIAG_RESET
#undef DIAG_BOLD
#undef DIAG_NOTE
#undef DIAG_WARNING
#undef DIAG_ERROR
	} // namespace Support
} // namespace MaryLang
