#pragma once

#include "AST.hpp"

namespace MaryLang
{
    namespace AbstractSyntaxTree
    {
		using namespace Lexer;

        struct ASTFactory
        {
			static CompoundStatement* GetCompoundStatement( Token const & token )
			{
				return new CompoundStatement( token );
			}

			static CheckAmongStatement* GetCheckAmongStatement( Token const & token, Expression * expr,
				Statement const * const body )
			{
				return new CheckAmongStatement( token, expr, body );
			}

			static IfStatement * GetIfStatement( Token const & token, 
				Expression const * const conditional_expr,
				Expression const * const other_expr, 
				Statement const * const else_statement,
				Statement const * const body )
			{
				return new IfStatement( token, conditional_expr, other_expr, 
					else_statement, body );
			}
        };

		struct Imports
		{

		};

		struct ParsedProgram
		{
			ParsedProgram(): source_program(), source_imports() {}
			~ParsedProgram() {}

			inline void Append( Statement const * const s )	  { source_program += s; }
			inline void Append( Imports const * const import ) { source_imports += import; }
			List<Statement> const & SourceProgram() const { return source_program; }
			List<Imports>   const & SourceImports() const { return source_imports; }
		private:
			List<Statement> source_program;
			List<Imports>   source_imports;
		};
    } // namespace AbstractSyntaxTree
} // namespace MaryLang
