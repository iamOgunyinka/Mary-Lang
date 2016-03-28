#pragma once

#include <vector>

namespace MaryLang
{
    namespace AbstractSyntaxTree
    {
        template<typename T>
        struct List
        {
            List( ): _list() {}
			List( std::vector< std::unique_ptr<T>> const & list ): _list( list ) {}
            List() {}
            
            List& operator+=( T const * t )
            {
                Append( t );
                return *this;
            }
            
            inline void Append( T const * t )
            {
				_list.push_back( std::unique_ptr<T>( t ) );
            }
			typedef typename std::vector<std::unique_ptr<T>>::iterator        iterator;
			typedef typename std::vector<std::unique_ptr<T>>::const_iterator  const_iterator;
			typedef typename std::vector<std::unique_ptr<T>>::size_type       size_type;
            
            inline size_type    Size()  { return _list.size(); }
            inline iterator     begin() { return _list.begin(); }
            inline iterator     end()   { return _list.end(); }
            
            inline const_iterator cbegin() const { return _list.cbegin(); }
            inline const_iterator cend() const { return _list.cend(); }
            
        private:
			std::vector<std::unique_ptr<T>> _list;
        }; // struct List
        
    } // namespace AbstractSyntaxTree
} // namespace MaryLang
