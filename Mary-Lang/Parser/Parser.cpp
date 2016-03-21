#include "Parser.hpp"
#include <cassert>

namespace MaryLang
{
	namespace Parser
	{
		Parser::Parser( Scanner & lex ): lexer( lex ) {}
		Parser::~Parser() {}

		void Parser::Expect( TokenType tt )
		{
			assert( tt == current_token->Type() && "expecting ..." );
			Accept( tt );
		}

		void Parser::Accept( TokenType tt )
		{
			NextToken();
		}

		void Parser::NextToken()
		{
			current_token.reset( next_token.release() );
			next_token.reset( new Token( lexer.GetNextToken() ) );
		}

		ParsedProgram *Parser::ParseProgram()
		{
			NextToken();
			NextToken();
			ParseSourceElement();
			Expect( TokenType::TK_EOF );
		}

		void Parser::ParseSourceElement()
		{
			while( current_token->Type() != TokenType::TK_EOF )
			{
				program->Append( ParseStatement() );
			}
		}

		Statement *Parser::ParseCompoundStatement()
		{
			Expect( TokenType::TK_LBRACE ); // consume "{"
			CompoundStatement *statement = ASTFactory::GetCompoundStatement( *current_token );
			while( current_token->Type() != TokenType::TK_RBRACE )
			{
				statement->Append( ParseStatement() );
			}
			Expect( TokenType::TK_RBRACE );
			return statement;
		}

		Statement *Parser::ParseCheckAmongStatement()
		{
			Token const token = *current_token;
			Expect( TokenType::TK_CHECK ); // consume "check"
			Expect( TokenType::TK_LPAREN ); // consume "("
			Expression *expression = ParseExpression();
			Expect( TokenType::TK_RPAREN ); //consume ")"
			Expect( TokenType::TK_AMONG ); // consume "among"
			Statement *body = ParseStatement();

			return ASTFactory::GetCheckAmongStatement( token, expression, body );
		}

		Statement *Parser::ParseConditionalStatement()
		{
			Token const token ( *current_token );
			Accept( TokenType::TK_IF ); // consume "if"
			Accept( TokenType::TK_LPAREN ); // consume "("
			Expression const * const conditional_expression = ParseExpression();
			Expression const *other_expression = nullptr;
			if( current_token->Type() == TokenType::TK_AMONG ){
				Accept( TokenType::TK_AMONG );
				other_expression = ParseExpression();
			}
			Expect( TokenType::TK_RPAREN );
			Statement const * const statement_body = ParseStatement();
			Statement const * else_body = nullptr;
			if( current_token->Type() == TokenType::TK_ELSE ){
				Accept( TokenType::TK_ELSE );
				else_body = ParseStatement();
			}
			Statement const * const if_statement = ASTFactory::GetIfStatement( token,
				conditional_expression, other_expression, statement_body, else_body );
		}

		/**** To-Do ****/
		Statement *Parser::ParseDeclarativeStatement()
		{

		}
		
		/**** To-Do ****/
		Statement *Parser::ParseIterativeStatement()
		{
			switch( current_token->Type() )
			{
			case TokenType::TK_FOR:

				break;
			case TokenType::TK_DO:

				break;
			case TokenType::TK_WHILE:

				break;

			}
		}

		Statement *Parser::ParseStatement()
		{
			switch ( current_token->Type() )
			{
			case TokenType::TK_LBRACE: // compound statement
				return ParseCompoundStatement();
			case TokenType::TK_CHECK: // check statement
				return ParseCheckAmongStatement();
			case TokenType::TK_IF: // conditional statement
				return ParseConditionalStatement();
			case TokenType::TK_FOR: // iterative statement
			case TokenType::TK_DO:
			case TokenType::TK_WHILE:
				return ParseIterativeStatement();
			case TokenType::TK_SEMICOLON: // empty statement
				Accept( TokenType::TK_SEMICOLON ); // eat semi-colon
				return nullptr;
			case TokenType::TK_RETURN: // break statement
			case TokenType::TK_CONTINUE:
			case TokenType::TK_LEAVE:
				return ParseJumpStatement();
			case TokenType::TK_VAR: // declarative statement
			case TokenType::TK_INT:
			case TokenType::TK_DOUBLE:
			case TokenType::TK_CLASS:
			case TokenType::TK_STRING:
			case TokenType::TK_FUNCTION:
				return ParseDeclarativeStatement();
			case TokenType::TK_ISIT: // labelled statement
				return ParseLabelledStatement();
			default:
				// could it be a class name followed by a variable name?
				if( next_token->Type() == TokenType::TK_IDENTIFIER ){
					return ParseDeclarativeStatement();
				} else {
					return ParseExpressionStatement();
				}
				break;
			}
		}
	} //namespace Parser
} // namespace MaryLang