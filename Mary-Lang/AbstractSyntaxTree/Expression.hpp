#pragma once

namespace MaryLang
{
    namespace AbstractSyntaxTree
    {
		using Lexer::Token;

		struct Expression: Locatable
		{
			Expression( Token const & token )
				:Locatable( token )
			{
			}
			virtual ~Expression() {}
			virtual void Analyze() const = 0;
			bool is_lvalue;
		};

		struct IllegalExpression: Expression
		{
			IllegalExpression( Token const & token )
				: Expression( token )
			{
			}
			~IllegalExpression() {}
			void Analyze() const;
		};

		struct ExpressionList: Expression, List<Expression>
		{
			explicit ExpressionList( Token const & token )
				: Expression( token ), List()
			{
			}
			explicit ExpressionList( Token const & token,
				std::vector<Expression const *> const & expressions )
				: Expression( token ), List( expressions )
			{
			}
			~ExpressionList() {}
			void Analyze() const;
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
			void Analyze() const;
		};

		struct StringLiteralExpression: Expression
		{
			StringLiteralExpression( Token const & token ): Expression( token )
			{
			}
			~StringLiteralExpression(){}
			void Analyze() const;
		};

		struct StringInterpolExpression: Expression
		{
			StringInterpolExpression( Token const & token ): Expression( token )
			{
			}
			~StringInterpolExpression(){}
			void Analyze() const;
		};

		struct ConditionalExpression: Expression
		{
			ConditionalExpression( Token const & token, Expression const * const cond,
				Expression const * const lhs, Expression const * const rhs )
				: Expression( token ), conditional_expression( cond ),
				lhs_expression( lhs ), rhs_expression( rhs )
			{
			}
			~ConditionalExpression() {}
			void Analyze() const;
			Expression const * const conditional_expression;
			Expression const * const lhs_expression;
			Expression const * const rhs_expression;
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
			BinaryExpression( Token const & token, Expression const * const lhs,
				Expression const * const rhs )
				: Expression( token ), lhs_expression( lhs ),
				rhs_expression( rhs )
			{
			}
			virtual ~BinaryExpression(){}
			virtual void Analyze() const = 0;

			Expression const * const lhs_expression;
			Expression const * const rhs_expression;
		};

		struct AssignmentExpression: BinaryExpression
		{
			AssignmentExpression( Token const & token, Expression const * const lhs,
				Expression const * const rhs )
				: BinaryExpression( token, lhs, rhs )
			{
			}
			~AssignmentExpression(){}
			void Analyze() const;
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
			SubscriptExpression( Token const & token, Expression const * const expr, 
				Expression const * const index )
				: PostfixExpression( token ), expression( expr ),
				index_expr( index )
			{
			}
			~SubscriptExpression(){}
			void Analyze() const;

			Expression const * const expression;
			Expression const * const index_expr;
		};

		struct DotExpression: PostfixExpression
		{
			DotExpression( Token const & token, Expression const * const expr,
				Token const & id )
				: PostfixExpression( token ), expression( expr ),
				token_id( id )
			{
			}
			~DotExpression(){}
			void Analyze() const;

			Expression const * const expression;
			Token	   const &		 token_id;
		};

		struct FunctionCall: PostfixExpression
		{
			FunctionCall( Token const & token, Expression const * const expr )
			:
			{}
		};
    } // namespace AbstractSyntaxTree
} //namespace MaryLang
