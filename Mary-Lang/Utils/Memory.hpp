#include <memory>

namespace MaryLang
{
    namespace Support
    {
#if __cplusplus != 201402L
        template<typename T>
        struct UniqueIf
		{
            typedef std::unique_ptr<T> SingleObject;
        };
        
        template< typename T>
        struct UniqueIf<T[]> 
		{
            typedef std::unique_ptr<T[]> UnknownBound;
        };
        
        template<typename T, size_t N>
        struct UniqueIf<T[N]> 
		{
            typedef void KnownBound;
        };
        
		template<typename T, typename ...Args>
        typename UniqueIf<T>::SingleObject make_unique( Args &&... args )
        {
            return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
        }
        
		template<typename T, typename ...Args>
        typename UniqueIf<T>::UnknownBound make_unique( size_t n )
        {
            typedef typename std::remove_extent<T>::type U;
            return std::unique_ptr<T>( new U[n]() );
        }
        
        template<typename T, typename ...Args>
        typename UniqueIf<T>::KnownBound make_unique( Args && ... );
#else
		using std::make_unique;
#endif
    }
} // namespace MaryLang
