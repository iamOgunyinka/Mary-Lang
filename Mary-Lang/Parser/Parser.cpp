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

		bool Parser::IsBuiltInType( TokenType tt )
		{
			return tt == TokenType::TK_VAR || tt == TokenType::TK_INT || tt == TokenType::TK_BOOLEAN
				|| tt == TokenType::TK_STRING || tt == TokenType::TK_DECLTYPE || tt == TokenType::TK_DOUBLE;
		}

		ParsedProgram *Parser::ParseProgram()
		{
			NextToken();
			NextToken();
			ParseImports();
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

		// To-Do
		void Parser::ParseImports()
		{

		}

		Statement *Parser::ParseCompoundStatement()
		{
			Expect( TokenType::TK_LBRACE ); // consume "{"
			CompoundStatement *statement = ASTFactory::GetCompoundStatement( *current_token );
			while( current_token->Type() != TokenType::TK_RBRACE )
			{
				statement->Append( ParseStatement() );
			}
			Accept( TokenType::TK_RBRACE ); // consume "}"
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

		Statement *Parser::ParseDeclarativeStatement()
		{
			Token token = *current_token;
			Declaration *declaration = nullptr;
			switch( current_token->Type() )
			{
			case TokenType::TK_CLASS:
				declaration = ParseClassDeclaration();
				break;
			case TokenType::TK_ENUM:
				declaration = ParseEnumDeclaration();
				break;
			default:
				declaration = ParseOtherDeclaration();
			}
			return declaration;
		}

		Statement *Parser::ParseIterativeStatement()
		{
			Statement *iterativeStatement = nullptr;
			switch( current_token->Type() )
			{
			case TokenType::TK_FOR:
				iterativeStatement = ParseForStatement();
				break;
			case TokenType::TK_DO:
				iterativeStatement = ParseDoWhileStatement();
				break;
			case TokenType::TK_WHILE:
			default:
				iterativeStatement = ParseWhileStatement();
				break;
			}
			return iterativeStatement;
		}

		Statement* Parser::ParseWhileStatement()
		{
			Accept( TokenType::TK_WHILE );
			Token const token = *current_token;
			Statement *statement = nullptr;

			if( current_token->Type() == TokenType::TK_LPAREN ){
				Accept( current_token->Type() );
			} else {
				error_messages.Propagate( *current_token, L"Expected an open parenthesis before the expression" );
			}
			Expression const * const expression = ParseExpression();
			Expect( TokenType::TK_RPAREN );
			statement = ParseStatement();

			return ASTFactory::GetWhileStatement( token, expression, statement );
		}

		Statement* Parser::ParseDoWhileStatement()
		{
			Token const token = *current_token;
			Accept( TokenType::TK_DO );
			Statement const * const statement = ParseStatement();
			Expect( TokenType::TK_WHILE );
			if( current_token->Type() != TokenType::TK_LPAREN ){
				error_messages.Propagate( token, L"Expected '(' before expression" );
			} else {
				Accept( TokenType::TK_LPAREN );
			}
			Expression const * const expression = ParseExpression();
			if( current_token->Type() != TokenType::TK_RPAREN ){
				error_messages.Propagate( *current_token, L"Expected ')' before expression" );
			} else {
				Accept( TokenType::TK_RPAREN );
			}
			Accept( TokenType::TK_SEMICOLON );
			return ASTFactory::GetDoWhileStatement( token, expression, statement );
		}

		Statement* Parser::ParseForStatement()
		{
			Token const token = *current_token;
			Accept( TokenType::TK_FOR ); // consume "for"
			Accept( TokenType::TK_LPAREN ); //consume "("

			Declaration const * declaration = nullptr;
			Expression  const * init_expression = nullptr;
			Expression  const * rhs_expression = nullptr;
			Expression  const * step = nullptr;
			Expression  const * condition = nullptr;

			if( IsBuiltInType( current_token->Type() ) || 
				( current_token->Type() == TokenType::TK_IDENTIFIER && next_token->Type() == TokenType::TK_IDENTIFIER ) )
			{
				declaration = ParseVariableDeclaration();
			} else {
				if( TokenType::TK_SEMICOLON != current_token->Type() ){
					init_expression = ParseExpression();
				} else if( TokenType::TK_SEMICOLON == current_token->Type() ) {
					Accept( TokenType::TK_SEMICOLON );
				} else {
					error_messages.Propagate( token, L"" );
					while( current_token->Type() != TokenType::TK_SEMICOLON ) NextToken();
				}
			}
			if( current_token->Type() == TokenType::TK_COLON ){ //for constructs like "for( var c : d )"
				Accept( current_token->Type() ); // consume ":"
				rhs_expression = ParseExpression();
			} else {
				Accept( TokenType::TK_SEMICOLON );
				if( current_token->Type() != TokenType::TK_SEMICOLON ){
					condition = ParseExpression();
				}
				Expect( TokenType::TK_SEMICOLON );
				if( current_token->Type() != TokenType::TK_RPAREN ){
					step = ParseExpression();
				}
			}
			Accept( TokenType::TK_RPAREN );
			Statement const * const statement = ParseStatement();

			if( rhs_expression == nullptr ){ // we have "for( : )" construct
				return ASTFactory::GetForInStatement( token, declaration, init_expression, rhs_expression, statement );
			} else {
				return ASTFactory::GetForStatement( token, declaration, init_expression, condition, step, statement );
			}
		}

		Declaration* Parser::ParseOtherDeclaration()
		{
			switch( current_token->Type() )
			{
			case TokenType::TK_FUNCTION:
				return ParseFunctionDeclaration();
			case TokenType::TK_NAMESPACE:
				return ParseNamespaceDeclaration();
			case TokenType::TK_VIRTUAL: 
				if( next_token->Type() != TokenType::TK_FUNCTION ){
					error_messages.Propagate( *current_token, L"Keyword virtual is only allowed in class definition \
															   with a function" );
				}
				return ParseFunctionDeclaration();
			case TokenType::TK_STATIC:
				return next_token->Type() == TokenType::TK_FUNCTION ? ParseFunctionDeclaration(): ParseVariableDeclaration();
			default:
				return ParseVariableDeclaration();
			}
		}

		Declaration* Parser::ParseNamespaceDeclaration()
		{
			Token const token = *current_token;
			Accept( TokenType::TK_NAMESPACE );
			std::unique_ptr<Token> namespace_name = nullptr;
			if( current_token->Type() == TokenType::TK_IDENTIFIER ){
				namespace_name.reset( new Token( *current_token ) );
				Accept( TokenType::TK_IDENTIFIER );
			}
			Statement * namespace_body = ParseCompoundStatement();
			return ASTFactory::GetNamespaceDeclaration( token, std::move( namespace_name ), namespace_body );
		}

		Declaration* Parser::ParseFunctionDeclaration()
		{
			Token const functional_specifier = *current_token;
			if( current_token->Type() == TokenType::TK_STATIC || current_token->Type() == TokenType::TK_VIRTUAL ){
				NextToken();
			}
			Token const token = *current_token;

			Accept( TokenType::TK_FUNCTION ); // consume "function"
			if( current_token->Type() != TokenType::TK_IDENTIFIER ){
				error_messages.Propagate( *current_token, L"Expected a function name after keyword function" );
				TokenType tt = current_token->Type();
				while( tt != TokenType::TK_LBRACE && tt != TokenType::TK_EOF ){
					NextToken();
					tt = current_token->Type();
				}
				return nullptr;
			}
			Token const function_name = *current_token;
			Accept( TokenType::TK_IDENTIFIER );
			Accept( TokenType::TK_LPAREN );
			ParameterlistDeclaration *parameter_list = nullptr;
			if( current_token->Type() == TokenType::TK_RPAREN ){ // no parameters
				Accept( TokenType::TK_RPAREN );
			} else { // we have parameters
				parameter_list = ParseParameterList();
			}
			Token trailing_function_property = *current_token;
			if( current_token->Type() == TokenType::TK_IDENTIFIER ){
				Accept( TokenType::TK_IDENTIFIER );
			}
			Token trailing_return_type = *current_token;
			if( current_token->Type() == TokenType::TK_ARROW ){
				Accept( TokenType::TK_ARROW ); // consume ->
				trailing_return_type = *current_token;
				Expect( TokenType::TK_IDENTIFIER );
			}
			Statement const * const function_body = ParseCompoundStatement();
			return ASTFactory::GetFunctionDeclaration( functional_specifier, token, function_name, parameter_list,
				trailing_function_property, trailing_return_type, function_body );
		}

		Declaration* Parser::ParseEnumDeclaration()
		{
			Token const token = *current_token;
			Expect( TokenType::TK_ENUM );

			std::unique_ptr<Token> enum_name = nullptr ;

			if( current_token->Type() == TokenType::TK_IDENTIFIER ){
				enum_name.reset( new Token( *current_token ) );
				Accept( TokenType::TK_IDENTIFIER );
			}
			Expect( TokenType::TK_LBRACE ); // consume "{"
			EnumDeclaration* enum_declaration = ASTFactory::GetEnumDeclaration( token, std::move( enum_name ) );

			std::unique_ptr<Token> enumerator_id, enumerator_value;

			while( current_token->Type() != TokenType::TK_EOF ){
				if( current_token->Type() == TokenType::TK_RBRACE ){
					break;
				}
				if( current_token->Type() != TokenType::TK_IDENTIFIER ){
					error_messages.Propagate( *current_token, L"Expected an identifier for enumerator" );
					while( current_token->Type() != TokenType::TK_RBRACE ) NextToken();
					break;
				}
				enumerator_id.reset( new Token( *current_token ) );
				Accept( TokenType::TK_IDENTIFIER );
				if( current_token->Type() == TokenType::TK_ASSIGN ){
					NextToken(); // consume "="
					if( current_token->Type() != TokenType::TK_INT ){
						error_messages.Propagate( *current_token, L"Expects a constant integer" );
						continue;
					}
					enumerator_value.reset( new Token( *current_token ) );
					Accept( TokenType::TK_INT );
				}
				enum_declaration->Append( std::move( enumerator_id ), std::move( enumerator_value ) );
				if( current_token->Type() != TokenType::TK_COMMA ) Accept( TokenType::TK_RBRACE );
				Accept( TokenType::TK_COMMA ); //consume ","
			}
			return enum_declaration;
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
			case TokenType::TK_NAMESPACE:
			case TokenType::TK_DECLTYPE:
				return ParseDeclarativeStatement();
			case TokenType::TK_ISIT: // labelled statement
				return ParseLabelledStatement();
			default:
				// could it be a class name followed by a variable name?
				if( next_token->Type() == TokenType::TK_IDENTIFIER ){
					return ParseDeclarativeStatement();
				} else { 
					// or a qualified ID like std.io.println( ), std.container.vector
					if( next_token->Type() == TokenType::TK_DOT ){
						std::vector<Token> tokens;
						while( next_token->Type() == TokenType::TK_DOT && current_token->Type() == TokenType::TK_IDENTIFIER )
						{
							tokens.push_back( *current_token );
							tokens.push_back( *next_token );

							NextToken();
							NextToken();
						}
						if( next_token->Type() == TokenType::TK_IDENTIFIER ){
							return ParseVariableDeclaration( tokens );
						} else {
							return ParseExpressionStatement();
						}
					}
					return ParseExpressionStatement();
				}
				break;
			}
		}
	} //namespace Parser
} // namespace MaryLang