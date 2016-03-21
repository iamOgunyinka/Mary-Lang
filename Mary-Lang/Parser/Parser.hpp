#pragma once

#include "../AbstractSyntaxTree/ASTFactory.hpp"
#include "../Scanner/Scanner.hpp"

namespace MaryLang
{
	namespace Parser
	{
		using namespace Lexer;
		using namespace AbstractSyntaxTree;

		struct Parser
		{
			Parser( Scanner & lex );
			~Parser();

			ParsedProgram *Parse();
		private:
			Scanner &lexer;
			std::unique_ptr<Token> current_token;
			std::unique_ptr<Token> next_token;
			ParsedProgram *program;

			void Accept( TokenType tt );
			void Expect( TokenType tt );
			void NextToken();
			ParsedProgram *ParseProgram();
			void ParseSourceElement();
			
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

		};
	} // namespace Parser
} //namespace MaryLang