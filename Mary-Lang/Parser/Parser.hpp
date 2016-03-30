#pragma once

#include "../AbstractSyntaxTree/ASTFactory.hpp"
#include "../Scanner/Scanner.hpp"
#include <deque>

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

			std::shared_ptr<ParsedProgram> Parse();
		private:
			std::unique_ptr<Token>		current_token;
			std::unique_ptr<Token>		next_token;
			std::deque<Token>			lookahead_tokens;
			std::shared_ptr<ParsedProgram> program;
			Scanner&					lexer;
			std::unique_ptr<Error>		error_messages;

			void Accept( TokenType tt );
			void Expect( TokenType tt );
			void NextToken();
			void ParseSourceElement();
			void ParseImports();
			bool IsBuiltInType( TokenType tt );
			
			std::shared_ptr<ParsedProgram> ParseProgram();
			std::unique_ptr<Expression> ParseAssignmentExpression();
			std::unique_ptr<Expression> ParseExpression();

			std::unique_ptr<Statement> ParseCompoundStatement();
			std::unique_ptr<Statement> ParseStatement();
			std::unique_ptr<Statement> ParseCheckAmongStatement();
			std::unique_ptr<Statement> ParseLabelledStatement();
			std::unique_ptr<Statement> ParseConditionalStatement();
			std::unique_ptr<Statement> ParseJumpStatement();
			std::unique_ptr<Statement> ParseIterativeStatement();
			std::unique_ptr<Statement> ParseDeclarativeStatement();

			std::unique_ptr<Statement> ParseWhileStatement();
			std::unique_ptr<Statement> ParseForStatement();
			std::unique_ptr<Statement> ParseDoWhileStatement();
			std::unique_ptr<Declaration> ParseEnumDeclaration();
			std::unique_ptr<Declaration> ParseClassDeclaration();
			std::unique_ptr<Declaration> ParseFunctionDeclaration();
			std::unique_ptr<Declaration> ParseOtherDeclaration();
			std::unique_ptr<Declaration> ParseNamespaceDeclaration();
			std::unique_ptr<Declaration> ParseVariableDeclaration();
			std::unique_ptr<ParameterlistDeclaration> ParseParameterList();
		};
	} // namespace Parser
} //namespace MaryLang