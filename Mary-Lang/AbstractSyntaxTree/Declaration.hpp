#pragma once

#include "Statement.hpp"

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

			virtual void Analyze() const final;
		};

		struct ParameterDeclaration: Declaration
		{
			ParameterDeclaration( Lexer::Token const & token, Identifier const * const identifier,
				TypeSpecifier const * const type )
				:   Declaration( token ), id( identifier ), type_specifier( type )
			{
			}
			~ParameterDeclaration(){}
			void Analyze() const final;
		private:
			Identifier      const * const id;
			TypeSpecifier   const * const type_specifier;
		};

		struct ParameterlistDeclaration
		{
			ParameterlistDeclaration( Lexer::Token const & token,
				std::vector<std::unique_ptr<ParameterDeclaration>> const & param_list )
				:   list( param_list )
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
			void Append( ParameterDeclaration const * const declaration ) { list.Append( declaration); }
		
			~ParameterlistDeclaration() {}
			virtual void Analyze() const final;

		private:
			List<ParameterDeclaration> list;
		};

		struct FunctionDeclaration: Declaration
		{
			FunctionDeclaration( Lexer::Token const & token, Lexer::Token const & specifier, 
				Lexer::Token const & name, ParameterlistDeclaration const * const param_list,
				Lexer::Token const & return_trailing_specifier, Lexer::Token const & type_specifier,
				Statement const * const body )
				:   Declaration( token ), function_specifier( specifier ), function_id( name ),
				function_trailing_specifier( return_trailing_specifier ), function_type_specifier( type_specifier ),
				parameter_list( param_list ), statement_body( body )
			{
			}
			~FunctionDeclaration() {}
			void Analyze() const final;
		private:
			Token					 const function_specifier;
			Token					 const function_id;
			Token					 const function_trailing_specifier;
			Token					 const function_type_specifier;
			ParameterlistDeclaration const * const parameter_list;
			Statement				 const * const statement_body;
		};

		struct ClassDeclaration: Declaration
		{
			ClassDeclaration( Lexer::Token const & token )
				: Declaration( token )
			{
			}
		};

		struct Enumerator
		{
			Enumerator( std::unique_ptr<Token> identifier ):
				enumerator_id( std::move( identifier ) ), enumerator_value( nullptr ) {}
			Enumerator( std::unique_ptr<Token> identifier, std::unique_ptr<Token> value )
				: enumerator_id( std::move( identifier ) ), enumerator_value( std::move( value ) )
			{
			}

		private:
			std::unique_ptr<Token>const enumerator_id;
			std::unique_ptr<Token>const enumerator_value;
		};

		struct EnumDeclaration: Declaration
		{
			EnumDeclaration( Token const & token, std::unique_ptr<Token> enum_id )
				: Declaration( token ), enum_name( std::move( enum_id ) )
			{
			}
			void Append( std::unique_ptr<Token> id, std::unique_ptr<Token> value )
			{
				enumerator_list.Append( new Enumerator( std::move( id ), std::move( value ) ) );
			}
			void Analyze() const ;
		private:
			std::unique_ptr<Token> enum_name;
			List<Enumerator> enumerator_list;
		};

		struct NamespaceDeclaration: Declaration
		{
			NamespaceDeclaration( Token const & token, std::unique_ptr<Token> namespace_name,
				Statement const * const namespace_body )
				: Declaration( token ), name( std::move( namespace_name ) ),
				body( namespace_body )
			{
			}
			~NamespaceDeclaration() {}
			void Analyze() const;
		private:
			std::unique_ptr<Token>	name;
			Statement const * const body;
		};
	} // namespace AbstractSyntaxTree
} // namespace MaryLang
