#pragma once

#include <cwctype>
#include <cwchar>

namespace Mary
{
	namespace Support
	{
		static wchar_t* mystrndup( wchar_t const *str, size_t n )
		{
			wchar_t *dup = new wchar_t[ n + 1 ];
#if defined( _WIN32 )
			int s = wcsncpy_s( dup, n + 1, str, n );
			if( s != 0 ) {
				delete []dup;
				dup = nullptr;
			}
			return dup;
#else
			return std::wcsncpy( dup, str, n );
#endif
		}
	} // namespace Support
}//namespace Mary