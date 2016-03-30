#pragma once

#include "AST.hpp"
#include "../Utils/Memory.hpp"

namespace MaryLang
{
	namespace AbstractSyntaxTree
	{
		using namespace Lexer;
		using namespace Support;

		struct ASTFactory
		{
			static std::unique_ptr<CompoundStatement> GetCompoundStatement( Token const & token )
			{
				return make_unique<CompoundStatement>( token );
			}

			static std::unique_ptr<CheckAmongStatement> GetCheckAmongStatement( Token const & token, 
				std::unique_ptr<Expression> expr, std::unique_ptr<Statement> body )
			{
				return make_unique<CheckAmongStatement>( token, std::move( expr ), std::move( body ) );
			}

			static std::unique_ptr<IfStatement> GetIfStatement( Token const & token, 
				std::unique_ptr<Expression> conditional_expr,
				std::unique_ptr<Expression> other_expr, 
				std::unique_ptr<Statement> else_statement,
				std::unique_ptr<Statement> body )
			{
				return make_unique<IfStatement>( token, std::move( conditional_expr ), std::move( other_expr ), 
					std::move( else_statement ), std::move( body ) );
			}
			static std::unique_ptr<WhileStatement> GetWhileStatement( Token const & token, std::unique_ptr<Expression> expression,
				std::unique_ptr<Statement> statement )
			{
				return make_unique<WhileStatement>( token, std::move( expression ), std::move( statement ) );
			}
			
			static std::unique_ptr<DoWhileStatement> GetDoWhileStatement( Token const & token, 
				std::unique_ptr<Expression> expression,
				std::unique_ptr<Statement> statement )
			{
				return make_unique<DoWhileStatement>( token, std::move( expression ), std::move( statement ) );
			}
			
			static std::unique_ptr<ForInStatement> GetForInStatement( Token const & token, std::unique_ptr<Declaration> declaration, 
				std::unique_ptr<Expression> init, std::unique_ptr<Expression> expression, 
				std::unique_ptr<Statement> statement )
			{
				return make_unique<ForInStatement>( token, std::move( declaration ), std::move( expression ), 
					std::move( init ), std::move( statement ) );
			}
			static std::unique_ptr<ForStatement> GetForStatement( Token const & token, std::unique_ptr<Declaration> declaration, 
				std::unique_ptr<Expression> initializer, std::unique_ptr<Expression> condition, 
				std::unique_ptr<Expression> stepping, std::unique_ptr<Statement> statement )
			{
				return make_unique<ForStatement>( token, std::move( declaration ), std::move( initializer ), 
					std::move( condition ), std::move( stepping ), std::move( statement ) );
			}
			
			static std::unique_ptr<Declaration> GetFunctionDeclaration( Token const & token, Token const & function_specifier, 
				Token const & function_name, std::unique_ptr<ParameterlistDeclaration> param_list,
				Token const & return_trailing_specifier, Token const & type_specifier, std::unique_ptr<Statement> body )
			{
				return make_unique<FunctionDeclaration>( token, function_specifier, function_name, std::move( param_list ),
					return_trailing_specifier, type_specifier, std::move( body ) );
			}

			static std::unique_ptr<EnumDeclaration> GetEnumDeclaration( Token const & token, std::unique_ptr<Token> enum_id )
			{
				return make_unique<EnumDeclaration>( token, std::move( enum_id ) );
			}

			static std::unique_ptr<NamespaceDeclaration>GetNamespaceDeclaration( Token const & token, 
				std::unique_ptr<Token> namespace_name, std::unique_ptr<Statement> namespace_body )
			{
				return make_unique<NamespaceDeclaration>( token, std::move( namespace_name ), std::move( namespace_body ) );
			}

			static std::unique_ptr<ContinueStatement> GetContinueStatement( Token const & token )
			{
				return make_unique<ContinueStatement>( token );
			}

			static std::unique_ptr<ReturnStatement> GetReturnStatement( Token const & token, std::unique_ptr<Expression> expr )
			{
				return make_unique<ReturnStatement>( token, std::move( expr ) );
			}

			static std::unique_ptr<LeaveStatement> GetLeaveStatement( Token const & token )
			{
				return make_unique<LeaveStatement>( token );
			}
		};

		struct Imports
		{

		};

		struct ParsedProgram
		{
			ParsedProgram(): source_program(), source_imports() {}
			~ParsedProgram() {}

			inline void Append( std::unique_ptr<Statement> s )	  { source_program.Append( std::move( s ) ); }
			inline void Append( std::unique_ptr<Imports> import ) { source_imports.Append( std::move( import ) ); }
			List<Statement> const & SourceProgram() const { return source_program; }
			List<Imports>   const & SourceImports() const { return source_imports; }
		private:
			List<Statement> source_program;
			List<Imports>   source_imports;
		};
	} // namespace AbstractSyntaxTree
} // namespace MaryLang
