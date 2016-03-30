#pragma once

#include "Statement.hpp"
#include "../Utils/Memory.hpp"

#define ANALYZE_DUMP_DECL \
	void Analyze() const; \
	void Dump() const

namespace MaryLang
{
	namespace CodeGeneration
	{
		struct CodeGen;
	}

	namespace AbstractSyntaxTree
	{
		// forward declarations
		struct Locatable;
		struct ValueTag;
		struct TypeSpecifier;
		struct Identifier;

		struct Declaration: Statement
		{
			Declaration( Lexer::Token const & token ): Statement( token )
			{
			}
			virtual ~Declaration() {}
			virtual void Analyze() const = 0;
		};

		struct VariableDeclaration: Declaration
		{
			VariableDeclaration( Lexer::Token const & token )
				: Declaration( token ), type_value( nullptr )
			{}
		private:
			ValueTag      const * const type_value; // what it points to
		};

		struct Identifier: Locatable
		{
			Identifier( Lexer::Token const & token ): Locatable( token ){}
			~Identifier() {}
			ANALYZE_DUMP_DECL;
		};

		struct ParameterDeclaration: Declaration
		{
			ParameterDeclaration( Lexer::Token const & token, Identifier const * const identifier,
				TypeSpecifier const * const type )
				:   Declaration( token ), id( identifier ), type_specifier( type )
			{
			}
			~ParameterDeclaration(){}
			ANALYZE_DUMP_DECL;
		private:
			Identifier      const * const id;
			TypeSpecifier   const * const type_specifier;
		};

		struct ParameterlistDeclaration
		{
			ParameterlistDeclaration( Lexer::Token const & token ): list()
			{
			}
			typedef List<ParameterDeclaration>::const_iterator const_iterator;
			typedef List<ParameterDeclaration>::iterator		iterator;
			typedef List<ParameterDeclaration>::size_type		size_type;

			iterator begin() { return list.begin(); }
			iterator end() { return list.end(); }
			const_iterator cbegin() const { return list.cbegin(); }
			const_iterator cend() const { return list.cend(); }
			size_type Size() { return list.Size(); }
			void Append( std::unique_ptr<ParameterDeclaration> declaration ) { list.Append( std::move( declaration )); }
		
			~ParameterlistDeclaration() {}
			ANALYZE_DUMP_DECL;
		private:
			List<ParameterDeclaration> list;
		};

		struct FunctionDeclaration: Declaration
		{
			FunctionDeclaration( Lexer::Token const & token, Lexer::Token const & specifier, 
				Lexer::Token const & name, std::unique_ptr<ParameterlistDeclaration> param_list,
				Lexer::Token const & return_trailing_specifier, Lexer::Token const & type_specifier,
				std::unique_ptr<Statement> body )
				:   Declaration( token ), function_specifier( specifier ), function_id( name ),
				function_trailing_specifier( return_trailing_specifier ), function_type_specifier( type_specifier ),
				parameter_list( std::move( param_list ) ), statement_body( std::move( body ) )
			{
			}
			~FunctionDeclaration() {}
			ANALYZE_DUMP_DECL;
		private:
			Token					 const function_specifier;
			Token					 const function_id;
			Token					 const function_trailing_specifier;
			Token					 const function_type_specifier;
			std::unique_ptr<ParameterlistDeclaration> const parameter_list;
			std::unique_ptr<Statement> const statement_body;
		};

		struct ClassDeclaration: Declaration
		{
			ClassDeclaration( Lexer::Token const & token )
				: Declaration( token )
			{
			}
			ANALYZE_DUMP_DECL;
		};

		struct Enumerator
		{
			Enumerator( Token const & identifier, std::unique_ptr<Token> value = nullptr )
				: enumerator_id( identifier ), enumerator_value( std::move( value ) )
			{
			}
			~Enumerator() {}
		private:
			Token					const enumerator_id;
			std::unique_ptr<Token>	const enumerator_value;
		};

		struct EnumDeclaration: Declaration
		{
			EnumDeclaration( Token const & token, std::unique_ptr<Token> enum_id )
				: Declaration( token ), enum_name( std::move( enum_id ) )
			{
			}
			void Append( Token const & id, std::unique_ptr<Token> value )
			{
				enumerator_list.Append( Support::make_unique<Enumerator>( id, std::move( value ) ) );
			}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Token>	const	enum_name;
			List<Enumerator>		enumerator_list;
		};

		struct NamespaceDeclaration: Declaration
		{
			NamespaceDeclaration( Token const & token, std::unique_ptr<Token> namespace_name,
				std::unique_ptr<Statement> namespace_body )
				: Declaration( token ), name( std::move( namespace_name ) ),
				body( std::move( namespace_body ) )
			{
			}
			~NamespaceDeclaration() {}
			ANALYZE_DUMP_DECL;
		private:
			std::unique_ptr<Token>		const name;
			std::unique_ptr<Statement>	const body;
		};
	} // namespace AbstractSyntaxTree
} // namespace MaryLang
