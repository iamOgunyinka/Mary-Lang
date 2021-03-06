#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace MaryLang
{
	namespace Support
	{
		struct Position
		{
			unsigned int	_line_number;
			unsigned int	_column_number;

			explicit Position( unsigned int line_number, unsigned int column_number )
                : _line_number( line_number ), _column_number( column_number )
            {
            }
            
            friend inline std::wostream & operator<<( std::wostream & os, Position const & pos )
			{
				return os << pos._line_number << L":" << pos._column_number;
			}

			inline void Dump() const
			{
				std::wcerr << *this << std::endl;
			}
		}; // Position
	} // namespace Support
} // namespace MaryLang
