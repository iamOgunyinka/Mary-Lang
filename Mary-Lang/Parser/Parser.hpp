#pragma once

#include "../AbstractSyntaxTree/ASTFactory.hpp"
#include "../Scanner/Scanner.hpp"

namespace MaryLang
{
	namespace Parser
	{
		using namespace Lexer;
		using namespace AbstractSyntaxTree;

		struct Error
		{
			Error(): error(){}

			inline void Propagate( Token const & token, wchar_t const *what )
			{ 
				error.push_back( std::make_pair( token, what ) ); 
			}
		private:
			std::vector<std::pair<Token, wchar_t const *>> error;
		};
		struct Parser
		{
			Parser( Scanner & lex );
			~Parser();

			ParsedProgram *Parse();
		private:
			Scanner &lexer;
			std::unique_ptr<Token> current_token;
			std::unique_ptr<Token> next_token;
			Error				   error_messages;
			ParsedProgram *program;

			void Accept( TokenType tt );
			void Expect( TokenType tt );
			bool IsBuiltInType( TokenType tt );
			void NextToken();
			ParsedProgram *ParseProgram();
			void ParseSourceElement();
			void ParseImports();

			Expression *ParseAssignmentExpression();
			Expression *ParseExpression();

			Statement *ParseCompoundStatement();
			Statement *ParseStatement();
			Statement *ParseCheckAmongStatement();
			Statement *ParseLabelledStatement();
			Statement *ParseConditionalStatement();
			Statement *ParseJumpStatement();
			Statement *ParseIterativeStatement();
			Statement *ParseExpressionStatement();
			Statement *ParseDeclarativeStatement();

			Statement *ParseWhileStatement();
			Statement *ParseForStatement();
			Statement *ParseDoWhileStatement();
			Declaration *ParseEnumDeclaration();
			Declaration *ParseClassDeclaration();
			Declaration *ParseFunctionDeclaration();
			Declaration *ParseOtherDeclaration();
			Declaration *ParseNamespaceDeclaration();
			Declaration *ParseVariableDeclaration( std::vector<Token> const & tokens = std::vector<Token>() );
			ParameterlistDeclaration *ParseParameterList();
		};
	} // namespace Parser
} //namespace MaryLang