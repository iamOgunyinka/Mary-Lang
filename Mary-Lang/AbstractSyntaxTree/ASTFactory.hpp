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
			static WhileStatement* GetWhileStatement( Token const & token, Expression const * const expression,
				Statement const * const statement )
			{
				return new WhileStatement( token, expression, statement );
			}
			static DoWhileStatement* GetDoWhileStatement( Token const & token, Expression const * const expression,
				Statement const * const statement )
			{
				return new DoWhileStatement( token, expression, statement );
			}
			static ForInStatement* GetForInStatement( Token const & token, Declaration const * const declaration, 
				Expression const * const init, Expression const * const expression, Statement const * const statement )
			{
				return new ForInStatement( token, declaration, expression, init, statement );
			}
			static ForStatement* GetForStatement( Token const & token, Declaration const * const declaration, 
				Expression const * const initializer, Expression const * const condition, 
				Expression const * const stepping, Statement const * const statement )
			{
				return new ForStatement( token, declaration, initializer, condition, stepping, statement );
			}
			static Declaration* GetFunctionDeclaration( Token const & token, Token const & function_specifier, 
				Token const & function_name, ParameterlistDeclaration const * const param_list,
				Token const & return_trailing_specifier, Token const & type_specifier, Statement const * const body )
			{
				return new FunctionDeclaration( token, function_specifier, function_name, param_list, return_trailing_specifier,
					type_specifier, body );
			}

			static EnumDeclaration* GetEnumDeclaration( Token const & token, std::unique_ptr<Token> enum_id )
			{
				return new EnumDeclaration( token, std::move( enum_id ) );
			}
			static NamespaceDeclaration *GetNamespaceDeclaration( Token const & token, std::unique_ptr<Token> namespace_name,
				Statement const * const namespace_body )
			{
				return new NamespaceDeclaration( token, std::move( namespace_name ), namespace_body );
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
