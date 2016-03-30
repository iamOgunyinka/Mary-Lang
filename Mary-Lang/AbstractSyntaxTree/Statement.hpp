#pragma once

#include "../Scanner/tokens.hpp"
#include "List.hpp"
#include "Expression.hpp"

#define ANALYZE_DUMP_DECL \
	void Analyze() const; \
	void Dump() const

namespace MaryLang
{
	// forward declarations
	namespace CodeGeneration
	{
		struct CodeGen;
	} // namespace CodeGeneration

	namespace AbstractSyntaxTree
	{
		struct Statement: Locatable
		{
			Statement( Lexer::Token const & token ): Locatable( token ) {}
			virtual ~Statement()  {}

			//virtual void Emit( CodeGeneration::CodeGen & cfg ) const = 0;
			virtual void Analyze() const = 0; //all analysis should be defined in the Semantic Analyzer directory
			virtual void Dump() const = 0;
		}; // struct Statement

		struct IllegalStatement: Statement
		{
			IllegalStatement( Lexer::Token & token ): Statement( token ) {}
			~IllegalStatement(){}

			ANALYZE_DUMP_DECL;
		};

		struct CaseOfStatement: Statement
		{
			CaseOfStatement( Lexer::Token const & token, std::unique_ptr<Expression> expression,
				std::unique_ptr<Statement> statement )
				: Statement( token ),
				condition_expression( std::move( expression ) ),
				statement_body( std::move( statement ) )
			{
			}
			~CaseOfStatement() { }
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const condition_expression;
			std::unique_ptr<Statement>  const statement_body;
		};

		struct IfStatement: Statement
		{
			IfStatement( Lexer::Token const & token, 
				std::unique_ptr<Expression> conditionalExpression,
				std::unique_ptr<Expression> expr,
				std::unique_ptr<Statement> statementBody, 
				std::unique_ptr<Statement> elseBody = nullptr )
				: Statement( token ), 
				condExpressionPart( std::move( conditionalExpression ) ), expression( std::move( expr ) ),
				thenStatementPart( std::move( statementBody ) ), elseStatementPart( std::move( elseBody ) )
			{
			}
			~IfStatement() { }
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const condExpressionPart;
			std::unique_ptr<Expression> const expression;
			std::unique_ptr<Statement>  const thenStatementPart;
			std::unique_ptr<Statement>  const elseStatementPart;
		}; // struct IfStatement

		// base class for all iterative statements
		struct IterativeStatement: Statement
		{
			IterativeStatement( Lexer::Token const & token, std::unique_ptr<Statement> statement )
				: Statement( token ), statement_body( std::move( statement ) )
			{
			}

			virtual ~IterativeStatement() { }
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Statement> const statement_body;
		};

		struct DoWhileStatement: IterativeStatement
		{
			DoWhileStatement( Lexer::Token const & token, std::unique_ptr<Expression> expression,
				std::unique_ptr<Statement> statement )
				: IterativeStatement( token, std::move( statement ) ), 
				conditional_expression( std::move( expression ) )
			{
			}
			~DoWhileStatement() {}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const conditional_expression;
		};

		struct WhileStatement: IterativeStatement
		{
			WhileStatement( Lexer::Token const & token, std::unique_ptr<Expression> expression,
				std::unique_ptr<Statement> statement )
				: IterativeStatement( token, std::move( statement )  ), 
				condition_expression( std::move( expression ) )
			{
			}
			~WhileStatement(){}
			ANALYZE_DUMP_DECL;
		protected:
			std::unique_ptr<Expression> const condition_expression;
		};

		struct Declaration; //forward declaration

		struct ForStatement: IterativeStatement
		{
			ForStatement( Lexer::Token const & token, std::unique_ptr<Declaration> declaration,
				std::unique_ptr<Expression> initializer, std::unique_ptr<Expression> condition,
				std::unique_ptr<Expression> step, std::unique_ptr<Statement> statement )
				:   IterativeStatement( token, std::move( statement ) ),
				initializing_declaration( std::move( declaration ) ),
				initializing_expression( std::move( initializer ) ),
				conditional_expression( std::move( condition ) ),
				stepping_expression( std::move( step ) )
			{
			}
			~ForStatement() { }
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Declaration> const initializing_declaration;
			std::unique_ptr<Expression>  const initializing_expression;
			std::unique_ptr<Expression>  const conditional_expression;
			std::unique_ptr<Expression>  const stepping_expression;
		};

		struct ForInStatement: IterativeStatement
		{
			ForInStatement( Lexer::Token const & token, std::unique_ptr<Declaration> declaration,
				std::unique_ptr<Expression> expr, std::unique_ptr<Expression> init, 
				std::unique_ptr<Statement> statement )
				: IterativeStatement( token, std::move( statement ) ),
				initializer( std::move( declaration ) ), lhs_expression( std::move( init ) ),
				rhs_expression( std::move( expr ) )
			{
			}
			virtual ~ForInStatement() {}
			ANALYZE_DUMP_DECL;
		protected:
			std::unique_ptr<Declaration> const initializer;
			std::unique_ptr<Expression>  const lhs_expression;
			std::unique_ptr<Expression>  const rhs_expression;
		};

		struct LabelStatement: Statement
		{
			LabelStatement( Lexer::Token const & token, std::unique_ptr<Statement> statement )
				:   Statement( token ), statement_body( std::move( statement ) )
			{
			}
			~LabelStatement() {}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Statement> statement_body;
		};

		struct ReturnStatement: Statement
		{
			ReturnStatement( Lexer::Token const & token, std::unique_ptr<Expression> expr )
				:   Statement( token ), expression( std::move( expr ) )
			{
			}
			~ReturnStatement() {}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const expression;
		};

		struct CheckAmongStatement: Statement
		{
			CheckAmongStatement( Lexer::Token const & token, std::unique_ptr<Expression> expr,
				std::unique_ptr<Statement> body )
				: Statement( token ), conditional_expression( std::move( expr ) ),
				statement_body( std::move( body ) )
			{
			}
			~CheckAmongStatement() {}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const conditional_expression;
			std::unique_ptr<Statement>  const statement_body;
		};
		struct ContinueStatement: Statement
		{
			ContinueStatement( Lexer::Token const & token ): Statement( token ){}
			~ContinueStatement() {}
			ANALYZE_DUMP_DECL;
		};

		struct LeaveStatement: Statement
		{
			LeaveStatement( Lexer::Token const & token ): Statement( token ){}
			~LeaveStatement() {}
			ANALYZE_DUMP_DECL;
		};

		struct CompoundStatement: Statement
		{
			CompoundStatement( Lexer::Token const & token )
				:   Statement( token )
			{
			}
			~CompoundStatement(){}
			ANALYZE_DUMP_DECL;

			typedef List<Statement>::const_iterator const_iterator;
			typedef List<Statement>::iterator		iterator;
			typedef List<Statement>::size_type		size_type;

			iterator begin() { return list.begin(); }
			iterator end() { return list.end(); }
			const_iterator cbegin() const { return list.cbegin(); }
			const_iterator cend() const { return list.cend(); }
			size_type Size() { return list.Size(); }
			void Append( std::unique_ptr<Statement> statement ) { list.Append( std::move( statement ) ); }
		private:
			List<Statement> list;
		};
	} // namespace AbstractSyntaxTree
}// namespace MaryLang
