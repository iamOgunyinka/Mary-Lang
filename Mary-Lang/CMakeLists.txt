cmake_minimum_required( VERSION 2.8 )

project( MaryLang )

# define some folders
get_filename_component( MARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
get_filename_component( MARY_LANG_DIR "${MARY_DIR}/Mary-Lang/Mary-Lang" PATH )

set( SCANNER_DIR ${MARY_LANG_DIR}/Scanner )
set( PARSER_DIR ${MARY_LANG_DIR}/Parser )
set( AST_DIR ${MARY_LANG_DIR}/AbstractSyntaxTree )
set( UTILS_DIR ${MARY_LANG_DIR}/Utils )

add_definitions( "-std=c++11" )

# define the sources of the compiler
set(SOURCES
    ${SCANNER_DIR}/Scanner.cpp
    ${SCANNER_DIR}/tokens.cpp
    ${MARY_LANG_DIR}/Mary.cpp
)

# configure the executable
include_directories(
    ${MARY_LANG_DIR}/Scanner/
    ${MARY_LANG_DIR}/Parser/
    ${MARY_LANG_DIR}/AbstractSyntaxTree/
    ${MARY_LANG_DIR}/Utils/
)

add_executable( MaryLang ${HEADER} ${SOURCES} )
