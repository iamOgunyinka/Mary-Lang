#pragma once

#include "../Scanner/tokens.hpp"
#include "List.hpp"
#include "Expression.hpp"

namespace Mary
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
            
            virtual void Emit( CodeGeneration::CodeGen & cfg ) const = 0;
            virtual void Analyze() const = 0; //all analysis should be defined in the Semantic Analyzer directory
        }; // struct Statement
        
        struct IllegalStatement: Statement
        {
            IllegalStatement( Lexer::Token & token ): Statement( token ) {}
            ~IllegalStatement(){}
            virtual void Emit( CodeGeneration::CodeGen & cfg ) const;
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
            IfStatement( Lexer::Token const & token, Expression const * const conditionalExpression,
				Statement const * const statementBody, 
                Statement const * const elseBody = nullptr )
				: Statement( token ), condExpressionPart( conditionalExpression ), 
				thenStatementPart( statementBody ), elseStatementPart( elseBody )
            {
            }
            ~IfStatement() { }
            void Analyze() const;
            void Emit( CodeGeneration::CodeGen & cfg ) const = 0;
        private:
            Expression const * const condExpressionPart;
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
                Expression const * const expr, Statement const * const statement )
            :   IterativeStatement( token, statement ),
                initializing_declaration( declaration ),
                expression( expr )
            {
            }
            virtual ~ForInStatement() {}
            virtual void Analyze() const;
        protected:
            Declaration const * const initializing_declaration;
            Expression  const * const expression;
        };
        
        struct ForEachIn: ForInStatement
        {
            ForEachIn( Lexer::Token const & token, Declaration const * const declaration,
                Expression const * const expr, Statement const * const statement )
            :   ForInStatement( token, declaration, expr, statement )
            {
            }
            ~ForEachIn() {}
            void Analyze() const final;
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
        
        struct SourceElements: List< Statement >
        {
            SourceElements( std::vector<Statement const *> const & statements )
				:   List( statements )
            {
            }
            virtual ~SourceElements() {}
            virtual void Analyze() const = 0;
        };
        
        struct CompoundStatement: Statement, SourceElements
        {
            CompoundStatement( Lexer::Token const & token, 
                std::vector<Statement const * const> const & statements )
            :   Statement( token ), SourceElements( statements )
            {
            }
            ~CompoundStatement(){}
            void Analyze() const final;
			void emit( CodeGeneration::CodeGen & codegen );
        };
    } // namespace AbstractSyntaxTree
}// namespace Mary
