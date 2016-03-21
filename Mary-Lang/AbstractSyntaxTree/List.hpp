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
            List( std::vector< T const * > const & list ): _list( list ) {}
            virtual ~List() {}
            
            List& operator+=( T const * t )
            {
                Append( t );
                return *this;
            }
            
            inline void Append( T const * t )
            {
                _list.push_back( t );
            }
            typedef typename std::vector<T const *>::iterator        iterator;
            typedef typename std::vector<T const *>::const_iterator  const_iterator;
            typedef typename std::vector<T const *>::size_type       size_type;
            
            inline size_type    Size()  { return _list.size(); }
            inline iterator     begin() { return _list.begin(); }
            inline iterator     end()   { return _list.end(); }
            
            inline const_iterator cbegin() const { return _list.cbegin(); }
            inline const_iterator cend() const { return _list.cend(); }
            
        private:
            std::vector< T const * > _list;
        }; // struct List
        
    } // namespace AbstractSyntaxTree
} // namespace MaryLang
