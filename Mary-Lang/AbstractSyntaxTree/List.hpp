#pragma once

#include <vector>
#include <memory>
#include "../Scanner/tokens.hpp"

namespace MaryLang
{
    namespace AbstractSyntaxTree
    {
        template<typename T>
        struct List
        {
            List( ): _list() {}
			List( std::vector<std::unique_ptr<T>> && list ): _list( std::move( list ) ) {}
            ~List() {}
            
			inline void Append( std::unique_ptr<T> t )
            {
				_list.push_back( std::move( t ) );
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

		using Lexer::Token;

		struct Printer;
		struct Locatable
		{
			Locatable( Token const & tk ): token( tk ) {}
			virtual void Dump() const = 0;
			virtual void Print( Printer & print ) const;
		private:
			Token const token;
		};
    } // namespace AbstractSyntaxTree
} // namespace MaryLang
