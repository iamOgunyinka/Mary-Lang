#pragma once

namespace Mary
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
        
        struct Declaration
        {
            Declaration( Lexer::Token const & token )
            {
            }
            virtual ~Declaration() {}
            virtual void Analyze() const;
        };
        // To-Do
        struct VariableDeclaration: Declaration
        {
            VariableDeclaration( Lexer::Token const & token ){}
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
        
        struct ParameterlistDeclaration: List<ParameterDeclaration>
        {
            ParameterlistDeclaration( Lexer::Token const & token,
                std::vector<ParameterDeclaration> const & param_list )
            :   List( param_list )
            {
            }
            ~ParameterlistDeclaration() {}
            void Analyze() const final;
        };
        
        struct FunctionDeclaration: Declaration
        {
            FunctionDeclaration( Lexer::Token const & token, TypeSpecifier const * const type,
                Identifier const * const identifier, CompoundStatement const * const statement,
                ParameterlistDeclaration const * const param_list )
            
            :   Declaration( token ), type_specifier( type ),
                function_id( identifier ), parameter_list( param_list ),
                statement_body( statement )
            {
            }
            ~FunctionDeclaration() {}
            void Analyze() const final;
        private:
            TypeSpecifier            const * const type_specifier;
            Identifier               const * const function_id;
            ParameterlistDeclaration const * const parameter_list;
            CompoundStatement        const * const statement_body;
        };
        
        struct ClassDeclaration:
        {
            
        };
    } // namespace AbstractSyntaxTree
} // namespace Mary
