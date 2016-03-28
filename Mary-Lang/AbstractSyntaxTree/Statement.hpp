#pragma once

#include "../Scanner/tokens.hpp"
#include "List.hpp"
#include "Expression.hpp"

namespace MaryLang
{
	// forward declarations
	namespace CodeGeneration
	{
		struct CodeGen;
	} // namespace CodeGeneration

	namespace AbstractSyntaxTree
	{
		struct Locatable
		{
			Locatable( Lexer::Token const & tk ): token( tk ) {}
			virtual ~Locatable() { }
		private:
			Lexer::Token token;
		}; // struct Locatable

		struct Statement: Locatable
		{
			Statement( Lexer::Token const & token ): Locatable( token ) {}
			virtual ~Statement()  {}

			//virtual void Emit( CodeGeneration::CodeGen & cfg ) const = 0;
			virtual void Analyze() const = 0; //all analysis should be defined in the Semantic Analyzer directory
		}; // struct Statement

		struct IllegalStatement: Statement
		{
			IllegalStatement( Lexer::Token & token ): Statement( token ) {}
			~IllegalStatement(){}

			void Analyze() const;
			//virtual void Emit( CodeGeneration::CodeGen & cfg ) const;
		};

		struct CaseOfStatement: Statement
		{
			CaseOfStatement( Lexer::Token const & token, Expression const * const expression,
				Statement const* const statement )
				: Statement( token ),
				condition_expression( expression ),
				statement_body( statement )
			{
			}
			~CaseOfStatement() { }
			void Analyze() const;
		private:
			Expression const * const condition_expression;
			Statement  const * const statement_body;
		};

		struct IfStatement: Statement
		{
			IfStatement( Lexer::Token const & token, 
				Expression const * const conditionalExpression,
				Expression const * const expr,
				Statement const * const statementBody, 
				Statement const * const elseBody = nullptr )
				: Statement( token ), 
				condExpressionPart( conditionalExpression ), expression( expr ),
				thenStatementPart( statementBody ), elseStatementPart( elseBody )
			{
			}
			~IfStatement() { }
			void Analyze() const;
			//void Emit( CodeGeneration::CodeGen & cfg ) const = 0;
		private:
			Expression const * const condExpressionPart;
			Expression const * const expression;
			Statement const  * const thenStatementPart;
			Statement const  * const elseStatementPart;
		}; // struct IfStatement

		// base class for all iterative statements
		struct IterativeStatement: Statement
		{
			IterativeStatement( Lexer::Token const & token, Statement const * const statement )
				: Statement( token ), statement_body( statement )
			{
			}

			virtual ~IterativeStatement() { }

			void Analyze() const = 0;
		private:
			Statement const * const statement_body;
		};

		struct DoWhileStatement: IterativeStatement
		{
			DoWhileStatement( Lexer::Token const & token, Expression const * const expression,
				Statement const * const statement )
				: IterativeStatement( token, statement ), conditional_expression( expression )
			{
			}
			~DoWhileStatement() {}

			void Analyze() const;
		private:
			Expression const * const conditional_expression;
		};

		struct WhileStatement: IterativeStatement
		{
			WhileStatement( Lexer::Token const & token, Expression const* const expression,
				Statement const * const statement )
				: IterativeStatement( token, statement ), condition_expression( expression )
			{
			}
			~WhileStatement(){}
			void Analyze() const final;
		protected:
			Expression const * const condition_expression;
		};

		struct Declaration; //forward declaration

		struct ForStatement: IterativeStatement
		{
			ForStatement( Lexer::Token const & token, Declaration const * const declaration,
				Expression const * const initializer, Expression const * const condition,
				Expression const * const step, Statement const * const statement )
				:   IterativeStatement( token, statement ),
				initializing_declaration( declaration ),
				initializing_expression( initializer ),
				conditional_expression( condition ),
				stepping_expression( step )
			{
			}
			~ForStatement() { }
			void Analyze() const final;
		private:
			Declaration const * const initializing_declaration;
			Expression  const * const initializing_expression;
			Expression  const * const conditional_expression;
			Expression  const * const stepping_expression;
		};

		struct ForInStatement: IterativeStatement
		{
			ForInStatement( Lexer::Token const & token, Declaration const * const declaration,
				Expression const * const expr, Expression const * const init, 
				Statement const * const statement )
				: IterativeStatement( token, statement ),
				initializer( declaration ), lhs_expression( init ),
				rhs_expression( expr )
			{
			}
			virtual ~ForInStatement() {}
			virtual void Analyze() const;
		protected:
			Declaration const * const initializer;
			Expression  const * const lhs_expression;
			Expression  const * const rhs_expression;
		};

		struct LabelStatement: Statement
		{
			LabelStatement( Lexer::Token const & token, Statement const * const statement )
				:   Statement( token ), statement_body( statement )
			{
			}
			~LabelStatement() {}
			void Analyze() const final;
		private:
			Statement const * const statement_body;
		};

		struct ReturnStatement: Statement
		{
			ReturnStatement( Lexer::Token const & token, Expression const * const expr )
				:   Statement( token ), expression( expr )
			{
			}
			~ReturnStatement() {}
			void Analyze() const final;
		private:
			Expression const * const expression;
		};

		struct CheckAmongStatement: Statement
		{
			CheckAmongStatement( Token const & token, Expression const * const expr,
				Statement const * const body )
				: Statement( token ), conditional_expression( expr ),
				statement_body( body )
			{
			}
			~CheckAmongStatement() {}
			void Analyze() const;

			Expression const * const conditional_expression;
			Statement  const * const statement_body;
		};
		struct ContinueStatement: Statement
		{
			ContinueStatement( Lexer::Token const & token ): Statement( token ){}
			~ContinueStatement() {}
			void Analyze() const final;
		};

		struct BreakStatement: Statement
		{
			BreakStatement( Lexer::Token const & token ): Statement( token ){}
			~BreakStatement() {}
			void Analyze() const final;
		};

		struct CompoundStatement: Statement
		{
			CompoundStatement( Lexer::Token const & token )
				:   Statement( token )
			{
			}
			~CompoundStatement(){}
			void Analyze() const final;
			void emit( CodeGeneration::CodeGen & codegen );

			typedef List<Statement>::const_iterator const_iterator;
			typedef List<Statement>::iterator		iterator;
			typedef List<Statement>::size_type		size_type;

			iterator begin() { return list.begin(); }
			iterator end() { return list.end(); }
			const_iterator cbegin() const { return list.cbegin(); }
			const_iterator cend() const { return list.cend(); }
			size_type Size() { return list.Size(); }
			void Append( Statement const * const statement ) { list.Append( statement ); }
		private:
			List<Statement> list;
		};
	} // namespace AbstractSyntaxTree
}// namespace MaryLang
