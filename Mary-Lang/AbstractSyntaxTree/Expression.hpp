#pragma once

#include "../Scanner/tokens.hpp"
#include "List.hpp"

#define ANALYZE_DUMP_DECL \
	void Analyze() const; \
	void Dump() const

namespace MaryLang
{
    namespace AbstractSyntaxTree
    {
		using Lexer::Token;
		struct Printer;

		struct Expression: Locatable
		{
			Expression( Token const & token )
				:Locatable( token )
			{
			}
			virtual ~Expression() {}
			virtual void Analyze() const = 0;
			virtual void Dump() const;
			bool is_lvalue;
		};

		struct IllegalExpression: Expression
		{
			IllegalExpression( Token const & token )
				: Expression( token )
			{
			}
			~IllegalExpression() {}
			ANALYZE_DUMP_DECL;
		};

		struct ExpressionList: Expression
		{
			ExpressionList( Token const & token ): Expression( token )
			{
			}
			~ExpressionList() {}
			ANALYZE_DUMP_DECL;

		private:
			List<Expression> expressions;
		};

		struct Variable: Expression
		{
			Variable( Token const & token ): Expression( token )
			{
			}
			~Variable(){}
			void Analyze() const;
		};

		struct Constant: Expression
		{
			Constant( Token const & token ): Expression( token )
			{
			}
			~Constant(){}
			ANALYZE_DUMP_DECL;
		};

		struct StringLiteralExpression: Expression
		{
			StringLiteralExpression( Token const & token ): Expression( token )
			{
			}
			~StringLiteralExpression(){}
			ANALYZE_DUMP_DECL;
		};

		struct StringInterpolExpression: Expression
		{
			StringInterpolExpression( Token const & token ): Expression( token )
			{
			}
			~StringInterpolExpression(){}
			ANALYZE_DUMP_DECL;
		};

		struct ConditionalExpression: Expression
		{
			ConditionalExpression( Token const & token, std::unique_ptr<Expression> cond,
				std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs )
				: Expression( token ), conditional_expression( std::move( cond ) ),
				lhs_expression( std::move( lhs ) ), rhs_expression( std::move( rhs ) )
			{
			}
			~ConditionalExpression() {}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const conditional_expression;
			std::unique_ptr<Expression> const lhs_expression;
			std::unique_ptr<Expression> const rhs_expression;
		};

		struct UnaryExpression: Expression
		{
			UnaryExpression( Token const & token ): Expression( token )
			{
			}
			virtual ~UnaryExpression(){}
			void Analyze() const = 0;
		};

		struct BinaryExpression: Expression
		{
			BinaryExpression( Token const & token, std::unique_ptr<Expression> lhs,
				std::unique_ptr<Expression> rhs )
				: Expression( token ), lhs_expression( std::move( lhs ) ),
				rhs_expression( std::move( rhs ) )
			{
			}
			virtual ~BinaryExpression(){}
			virtual void Analyze() const = 0;
			virtual void Dump() const;
		private:
			std::unique_ptr<Expression> const lhs_expression;
			std::unique_ptr<Expression> const rhs_expression;
		};

		struct AssignmentExpression: BinaryExpression
		{
			AssignmentExpression( Token const & token, std::unique_ptr<Expression> lhs,
				std::unique_ptr<Expression> rhs )
				: BinaryExpression( token, std::move( lhs), std::move( rhs ) )
			{
			}
			~AssignmentExpression(){}
			ANALYZE_DUMP_DECL;
		};

		struct PostfixExpression: UnaryExpression
		{
			PostfixExpression( Token const &token ): UnaryExpression( token )
			{
			}
			virtual ~PostfixExpression(){}
		};

		struct SubscriptExpression: PostfixExpression
		{
			SubscriptExpression( Token const & token, std::unique_ptr<Expression> expr, 
				std::unique_ptr<Expression> index )
				: PostfixExpression( token ), expression( std::move( expr ) ),
				index_expr( std::move( index ) )
			{
			}
			~SubscriptExpression(){}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const expression;
			std::unique_ptr<Expression> const index_expr;
		};

		struct DotExpression: PostfixExpression
		{
			DotExpression( Token const & token, std::unique_ptr<Expression> expr,
				Token const & id )
				: PostfixExpression( token ), expression( std::move( expr ) ),
				token_id( id )
			{
			}
			~DotExpression(){}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Expression> const expression;
			Token	   const &		 token_id;
		};
    } // namespace AbstractSyntaxTree
} //namespace MaryLang
