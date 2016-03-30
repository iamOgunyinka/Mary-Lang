# Mary-Lang
Mary-Lang is a gradual typed programming language that borrows dynamism from Python and JS, syntax from C# and C++ with a whole lot of functionalities from other popular programming languages. Its syntax will continue to change until I have it perfected, e.g. should parameter types have type declaration like:
    
    static function( a : int, b : MyStruct ) const -> ReturnType { ... }
    // OR
    static function( int a, MyStruct b ) const -> ReturnType { ... }
    // OR
    static function( a, b ) const -> ReturnType { ... }

The current plan is to make the language use an ahead-of-time JIT.
