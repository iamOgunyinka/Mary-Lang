# Mary-Lang
Mary-Lang is a static typed programming language that borrows dynamism from Python and JS, syntax from C# and C++ with a whole lot of functionalities from other popular programming languages. Its syntax will continue to change until I have it perfected. Right now, some of its syntax appear like so:

    namespace fts {
    // function definition looks like this.
        function [ static ] 
        PrintVectorElements<T>( container : std::vector<T>, sep : string ) -> void
        { }

    // type alias
    using std::pair<int, int> as IntPair;

    /* Declarations */

    // array of 10x10 integer pairs
    var name : IntPair[10, 10];

    var a = new int( 0 ); // shared_ptr to an int.

    var b = new int[5]; // shared_ptr to five ints.

    var c : double*, d, e; // c, d, and e are shared_ptr to double

    var e : ( std::vector<int>, string ) -> void;

    e = PrintVectorElements<int>; // OK.

    // Error, lambda with different signature. 
    e = @( con : std::vector<int>, t : int )-> int { return 0; };

    using std::IO;
    @{ IO::PrintLn( "{name} the vampire slayer!" ); }(); // Warning, name is undeclared

    class AClass<T> {
        public class BClass {
            var t : T;
        };
        private var bc : BClass;
        public function AClass( c : T ){
            bc = c;
        }
        implement operator+ for( AClass a, AClass b ) -> T {
            return AClass( a.bc.t + b.bc.t );
       }
    };
    }

    using fts::AClass<int> as AC;

    function main() -> int 
    {
    var aclass : fts::AClass<int> = AC( 20 ), aclass_2 = AC( 40 );
    var aclass_3 = aclass + aclass_2;
    return 0;
    }

The current plan is to make the language use an ahead-of-time JIT.
