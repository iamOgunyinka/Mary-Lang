#include "Parser.hpp"
#include <cassert>

namespace MaryLang
{
	namespace Parser
	{
		Parser::Parser( Scanner & lex ): lexer( lex ) {}
		Parser::~Parser() {}

		inline void Parser::Expect( TokenType tt )
		{
			assert( tt == current_token->Type() && "expecting ..." );
			Accept( tt );
		}

		inline void Parser::Accept( TokenType tt )
		{
			NextToken();
		}

		void Parser::NextToken()
		{
			current_token.reset( next_token.release() );
			if( lookahead_tokens.size() == 0 ){
				next_token.reset( new Token( lexer.GetNextToken() ) );
			} else {
				next_token.reset( new Token( lookahead_tokens.front() ) );
				lookahead_tokens.pop_front();
			}
		}

		inline bool Parser::IsBuiltInType( TokenType tt )
		{
			return tt == TokenType::TK_VAR || tt == TokenType::TK_INT || tt == TokenType::TK_BOOLEAN
				|| tt == TokenType::TK_STRING || tt == TokenType::TK_DECLTYPE || tt == TokenType::TK_DOUBLE;
		}

		// To-Do. List all C++'s operator precedence rules.
		Parser::PrecAssocPair Parser::GetPrecedence( TokenType tt )
		{
			switch( tt )
			{
			case TokenType::TK_ADD:
			case TokenType::TK_SUB: return PrecAssocPair( 1, Associativity::LEFT_ASSOC );
			case TokenType::TK_MUL:
			case TokenType::TK_DIV: return PrecAssocPair( 2, Associativity::LEFT_ASSOC );
			case TokenType::TK_EXP:	return PrecAssocPair( 3, Associativity::RIGHT_ASSOC );
			default: return PrecAssocPair( -1, Associativity::NONE_ASSOC );
			}
		}

		std::shared_ptr<ParsedProgram> Parser::ParseProgram()
		{
			NextToken();
			NextToken();
			ParseImports();
			ParseSourceElement();
			Expect( TokenType::TK_EOF );
			return program;
		}

		void Parser::ParseSourceElement()
		{
			program.reset( new ParsedProgram );
			while( current_token->Type() != TokenType::TK_EOF )
			{
				program->Append( ParseStatement() );
			}
		}

		// To-Do -> Still contemplating on what to use:Python's import...from OR C#'s "uses" or use include...from
		void Parser::ParseImports()
		{

		}

		std::unique_ptr<Statement> Parser::ParseCompoundStatement()
		{
			Expect( TokenType::TK_LBRACE ); // consume "{"
			auto statement = ASTFactory::GetCompoundStatement( *current_token );
			while( current_token->Type() != TokenType::TK_EOF && current_token->Type() != TokenType::TK_RBRACE )
			{
				statement->Append( ParseStatement() );
			}
			Accept( TokenType::TK_RBRACE ); // consume "}"
			return std::move( statement );
		}

		std::unique_ptr<Statement> Parser::ParseCheckAmongStatement()
		{
			Token const token = *current_token;
			Expect( TokenType::TK_CHECK ); // consume "check"
			Expect( TokenType::TK_LPAREN ); // consume "("
			std::unique_ptr<Expression>expression = ParseExpression();
			Expect( TokenType::TK_RPAREN ); //consume ")"
			Expect( TokenType::TK_AMONG ); // consume "among"
			auto statement_body = ParseCompoundStatement();

			return ASTFactory::GetCheckAmongStatement( token, std::move( expression ), std::move( statement_body ) );
		}

		std::unique_ptr<Statement> Parser::ParseConditionalStatement()
		{
			Token const token ( *current_token );
			Accept( TokenType::TK_IF ); // consume "if"
			Expect( TokenType::TK_LPAREN ); // consume "("
			std::unique_ptr<Expression> conditional_expression = ParseExpression();
			std::unique_ptr<Expression> other_expression = nullptr;
			if( current_token->Type() == TokenType::TK_AMONG ){
				Accept( TokenType::TK_AMONG );
				other_expression = ParseExpression();
			}
			Expect( TokenType::TK_RPAREN );
			std::unique_ptr<Statement> statement_body = ParseStatement();
			std::unique_ptr<Statement> else_body = nullptr;
			if( current_token->Type() == TokenType::TK_ELSE ){
				Accept( TokenType::TK_ELSE );
				else_body = ParseStatement();
			}
			return ASTFactory::GetIfStatement( token, std::move( conditional_expression ), 
				std::move( other_expression ), std::move( statement_body ), std::move( else_body ) );
		}

		inline std::unique_ptr<Statement> Parser::ParseDeclarationStatement()
		{
			return ASTFactory::GetDeclarationStatement( *current_token, ParseDeclaration() );
		}

		inline std::unique_ptr<Declaration> Parser::ParseDeclaration()
		{
			switch( current_token->Type() )
			{
			case TokenType::TK_CLASS:
				return ParseClassDeclaration();
			case TokenType::TK_ENUM:
				return ParseEnumDeclaration();
			default:
				return ParseOtherDeclaration();
			}
		}

		inline std::unique_ptr<Statement> Parser::ParseIterativeStatement()
		{
			switch( current_token->Type() )
			{
			case TokenType::TK_FOR:
				return ParseForStatement();
			case TokenType::TK_DO:
				return ParseDoWhileStatement();
			default:
				return ParseWhileStatement();
			}
		}

		std::unique_ptr<Statement> Parser::ParseWhileStatement()
		{
			Token const token = *current_token;

			Accept( TokenType::TK_WHILE );
			if( current_token->Type() == TokenType::TK_LPAREN ){
				Accept( current_token->Type() );
			} else {
				error_messages->Propagate( *current_token, L"Expected an opening parenthesis before the expression" );
			}
			std::unique_ptr<Expression> expression = ParseExpression();
			Expect( TokenType::TK_RPAREN );
			auto statement = ParseStatement();

			return ASTFactory::GetWhileStatement( token, std::move( expression ), std::move( statement ) );
		}

		std::unique_ptr<Statement> Parser::ParseDoWhileStatement()
		{
			Token const token = *current_token;
			Accept( TokenType::TK_DO );
			std::unique_ptr<Statement> statement = ParseStatement();
			Expect( TokenType::TK_WHILE );
			if( current_token->Type() != TokenType::TK_LPAREN ){
				error_messages->Propagate( token, L"Expected '(' before expression" );
			} else {
				Accept( TokenType::TK_LPAREN );
			}
			std::unique_ptr<Expression> expression = ParseExpression();
			if( current_token->Type() != TokenType::TK_RPAREN ){
				error_messages->Propagate( *current_token, L"Expected ')' before expression" );
			} else {
				Accept( TokenType::TK_RPAREN );
			}
			Accept( TokenType::TK_SEMICOLON );
			return ASTFactory::GetDoWhileStatement( token, std::move( expression ), std::move( statement ) );
		}

		std::unique_ptr<Statement> Parser::ParseForStatement()
		{
			Token const token = *current_token;
			Accept( TokenType::TK_FOR ); // consume "for"
			Accept( TokenType::TK_LPAREN ); //consume "("

			std::unique_ptr<Declaration> declaration = nullptr;
			std::unique_ptr<Expression> init_expression = nullptr;
			std::unique_ptr<Expression> rhs_expression = nullptr;
			std::unique_ptr<Expression> step = nullptr;
			std::unique_ptr<Expression> condition = nullptr;

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
					error_messages->Propagate( token, L"" );
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
			std::unique_ptr<Statement> statement = ParseStatement();

			if( rhs_expression == nullptr ){ // we have "for( : )" construct
				return ASTFactory::GetForInStatement( token, std::move( declaration ), std::move( init_expression ),
					std::move( rhs_expression ), std::move( statement ) );
			} else {
				return ASTFactory::GetForStatement( token, std::move( declaration ), std::move( init_expression ), 
					std::move( condition ), std::move( step ), std::move( statement ) );
			}
		}

		std::unique_ptr<Declaration> Parser::ParseOtherDeclaration()
		{
			switch( current_token->Type() )
			{
			case TokenType::TK_FUNCTION:
				return ParseFunctionDeclaration();
			case TokenType::TK_NAMESPACE:
				return ParseNamespaceDeclaration();
			case TokenType::TK_VIRTUAL: 
				if( next_token->Type() != TokenType::TK_FUNCTION )
				{
					error_messages->Propagate( *current_token, L"Keyword virtual is only allowed in class definition \
																with a function" );
				}
				return ParseFunctionDeclaration();
			case TokenType::TK_STATIC:
				return next_token->Type() == TokenType::TK_FUNCTION ? ParseFunctionDeclaration(): ParseVariableDeclaration();
			default:
				return ParseVariableDeclaration();
			}
		}

		std::unique_ptr<Declaration> Parser::ParseNamespaceDeclaration()
		{
			Token const token = *current_token;
			Accept( TokenType::TK_NAMESPACE );
			std::unique_ptr<Token> namespace_name = nullptr;
			if( current_token->Type() == TokenType::TK_IDENTIFIER ){
				namespace_name.reset( new Token( *current_token ) );
				Accept( TokenType::TK_IDENTIFIER );
			}
			std::unique_ptr<Statement> namespace_body = ParseCompoundStatement();
			return ASTFactory::GetNamespaceDeclaration( token, std::move( namespace_name ), std::move( namespace_body ) );
		}

		std::unique_ptr<Declaration> Parser::ParseFunctionDeclaration()
		{
			Token const functional_specifier = *current_token;
			if( current_token->Type() == TokenType::TK_STATIC || current_token->Type() == TokenType::TK_VIRTUAL ){
				NextToken();
			}
			Token const token = *current_token;

			Accept( TokenType::TK_FUNCTION ); // consume "function"
			if( current_token->Type() != TokenType::TK_IDENTIFIER ){
				error_messages->Propagate( *current_token, L"Expected a function name after keyword function" );
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
			std::unique_ptr<ParameterlistDeclaration> parameter_list = nullptr;
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
			std::unique_ptr<Statement> function_body = ParseCompoundStatement();
			return ASTFactory::GetFunctionDeclaration( functional_specifier, token, function_name, std::move( parameter_list ),
				trailing_function_property, trailing_return_type, std::move( function_body ) );
		}

		// To-Do -> Implement predence climbing as used in Clang.
		std::unique_ptr<Expression> Parser::ParseBinaryExpression()
		{

		}

		std::unique_ptr<Expression> Parser::ParseConditionalExpression()
		{
			auto conditional_expression = ParseBinaryExpression();
			if( current_token->Type() == TokenType::TK_QMARK ){
				Token const token = *current_token;
				Accept( TokenType::TK_QMARK );
				auto lhs_expression = ParseExpression();
				Accept( TokenType::TK_COLON );
				return Support::make_unique<ConditionalExpression>( token, std::move( conditional_expression ), 
					std::move( lhs_expression ), ParseConditionalExpression() );
			}
			return std::move( conditional_expression );
		}

		std::unique_ptr<Expression> Parser::ParseExpression()
		{
			Token const token = *current_token;
			auto expr = ParseAssignmentExpression();
			if( current_token->Type() != TokenType::TK_COMMA ){
				return std::move( expr );
			}
			std::vector<std::unique_ptr<Expression>> expressions;
			expressions.push_back( std::move( expr ) );
			while( TokenType::TK_COMMA == current_token->Type() ){
				Accept( TokenType::TK_COMMA );
				expressions.push_back( ParseAssignmentExpression() );
			}

			return ASTFactory::GetExpressionList( token, std::move( expressions ) );
		}

		std::unique_ptr<Expression> Parser::ParseAssignmentExpression()
		{
			auto lhs_expression = ParseConditionalExpression();

			switch( current_token->Type() )
			{
			case TokenType::TK_ASSIGN:
			case TokenType::TK_MULEQL:
			case TokenType::TK_DIVEQL:
			case TokenType::TK_MODASSIGN:
			case TokenType::TK_ADDEQL:
			case TokenType::TK_SUBEQL:
			case TokenType::TK_LSASSIGN:
			case TokenType::TK_RSHIFT:
			case TokenType::TK_ANDEQL:
			case TokenType::TK_XORASSIGN:
			case TokenType::TK_OREQL:
				{
					Token const token = *current_token;
					Accept( current_token->Type() );
					return ASTFactory::GetAssignmentExpression( token, std::move( lhs_expression ), ParseConditionalExpression() );
				}
			default: return std::move( lhs_expression );
			}
		}

		// To-Do -> How do you parse qualified ID for names?
		std::unique_ptr<Declaration> Parser::ParseClassDeclaration()
		{
			Token const token = *current_token;
			Expect( TokenType::TK_CLASS );
			Token const class_name = *current_token;
			Expect( TokenType::TK_IDENTIFIER );
			if( current_token->Type() == TokenType::TK_EXTENDS ){
				std::deque<Token> base_classes;
				Accept( TokenType::TK_EXTENDS );
				/* expect an identifier */
				while( current_token->Type() == TokenType::TK_COMMA ){
					// To-Do -> expect comma separated base class names
				}
			}
			Accept( TokenType::TK_LBRACE );

			std::unique_ptr<ClassDeclaration> class_declaration = ASTFactory::GetClassDeclaration( token );

			while( TokenType::TK_RBRACE != current_token->Type() )
			{
				std::unique_ptr<Token> access_specifier = nullptr;
				switch( current_token->Type() )
				{
				case TokenType::TK_PUBLIC:
				case TokenType::TK_PRIVATE:
				case TokenType::TK_PROTECTED:
					access_specifier = make_unique<Token>( *current_token ); // let it fall to default case anyway
				default:
					class_declaration->Append( std::move( access_specifier ), ParseDeclaration() );
					break;
				}
			}
			Accept( TokenType::TK_RBRACE );
			Accept( TokenType::TK_SEMICOLON );

			return std::move( class_declaration );
		}

		std::unique_ptr<Declaration> Parser::ParseEnumDeclaration()
		{
			Token const token = *current_token;
			Expect( TokenType::TK_ENUM );

			std::unique_ptr<Token> enum_name = nullptr ;

			if( current_token->Type() == TokenType::TK_IDENTIFIER ){
				enum_name = make_unique<Token>( *current_token );
				Accept( TokenType::TK_IDENTIFIER );
			}
			Expect( TokenType::TK_LBRACE ); // consume "{"
			std::unique_ptr<EnumDeclaration> enum_declaration = ASTFactory::GetEnumDeclaration( token, std::move( enum_name ) );

			while( current_token->Type() != TokenType::TK_EOF ){
				if( current_token->Type() == TokenType::TK_RBRACE ){
					break;
				}
				if( current_token->Type() != TokenType::TK_IDENTIFIER ){
					error_messages->Propagate( *current_token, L"Expected an identifier for enumerator" );
					while( current_token->Type() != TokenType::TK_RBRACE ) NextToken();
					break;
				}

				Token const enumerator_id = *current_token;
				std::unique_ptr<Token> enumerator_value = nullptr;

				Accept( TokenType::TK_IDENTIFIER );

				if( current_token->Type() == TokenType::TK_ASSIGN ){
					Accept( TokenType::TK_ASSIGN ); // consume "="
					if( current_token->Type() != TokenType::TK_INT ){
						error_messages->Propagate( *current_token, L"Expects a constant integer" );
						NextToken();
					} else {
						enumerator_value = make_unique<Token>( *current_token );
						Accept( TokenType::TK_INT );
					}
				}
				enum_declaration->Append( enumerator_id, std::move( enumerator_value ) );
				if( current_token->Type() != TokenType::TK_COMMA ) Accept( TokenType::TK_RBRACE );
				Accept( TokenType::TK_COMMA ); //consume ","
			}
			return std::move( enum_declaration );
		}

		std::unique_ptr<Statement> Parser::ParseJumpStatement()
		{
			auto token = *current_token;
			switch( current_token->Type() )
			{
			case TokenType::TK_CONTINUE:
				Accept( TokenType::TK_CONTINUE );
				Accept( TokenType::TK_SEMICOLON );
				return ASTFactory::GetContinueStatement( token );
			case TokenType::TK_LEAVE:
				Accept( TokenType::TK_LEAVE );
				Accept( TokenType::TK_SEMICOLON );
				return ASTFactory::GetLeaveStatement( token );
			default:
				Accept( TokenType::TK_RETURN );
				auto expression = ParseExpression();
				return ASTFactory::GetReturnStatement( token, std::move( expression ) );
			}
		}

		inline std::unique_ptr<Statement> Parser::ParseExpressionStatement()
		{
			return ASTFactory::GetExpressionStatement( *current_token, ParseExpression() );
		}

		std::unique_ptr<Statement> Parser::ParseLabelledStatement()
		{
			auto token = *current_token;
			Accept( TokenType::TK_ISIT );
			std::unique_ptr<Token> value = nullptr;
			switch( current_token->Type() )
			{
			case TokenType::TK_TRUE:
			case TokenType::TK_FALSE:
			case TokenType::TK_INT:
			case TokenType::TK_STRLITERAL:
			case TokenType::TK_STRLITINTERPOL:
				value = make_unique<Token>( *current_token );
				break;
			default:
				error_messages->Propagate( *current_token, L"Invalid value supplied for label" );
				return nullptr;
			}
			Accept( TokenType::TK_COLON );
			return ASTFactory::GetLabelStatement( token, std::move( value ) );
		}

		std::unique_ptr<Statement> Parser::ParseStatement()
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
			case TokenType::TK_RETURN: // jump statement
			case TokenType::TK_CONTINUE:
			case TokenType::TK_LEAVE:
				return ParseJumpStatement();
			case TokenType::TK_VAR: // declaration statement
			case TokenType::TK_INT:
			case TokenType::TK_DOUBLE:
			case TokenType::TK_CLASS:
			case TokenType::TK_STRING:
			case TokenType::TK_FUNCTION:
			case TokenType::TK_STATIC:
			case TokenType::TK_VIRTUAL:
			case TokenType::TK_NAMESPACE:
			case TokenType::TK_DECLTYPE:
				return ParseDeclarationStatement();
			case TokenType::TK_ISIT: // labelled statement
				return  ParseLabelledStatement();
			case TokenType::TK_IDENTIFIER:
			default:
				return ParseExpressionStatement();
				break;
			}
		}
	} //namespace Parser
} // namespace MaryLang