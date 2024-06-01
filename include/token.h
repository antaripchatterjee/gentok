#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stddef.h>
#include <stdbool.h>

enum TOKENTYPE_E {
    T_TOKEN_INVALID = -1,
    T_TOKEN_UNKNOWN,
    T_WHITESPACE_SPACE,
    T_WHITESPACE_HORIZONTAL_TAB,
    T_WHITESPACE_VERTICAL_TAB, 
    T_WHITESPACE_FORM_FEED,
    T_WHITESPACE_CARRIAGE_RETURN,
    T_WHITESPACE_LINE_FEED,
    T_OPERAND_IDENTIFIER,
    T_OPERAND_STRING_VALUE,
    T_OPERAND_STRING_TEMPLATE,
    T_OPERAND_BINARY_INTEGER,
    T_OPERAND_OCTAL_INTEGER,
    T_OPERAND_DECIMAL_INTEGER,
    T_OPERAND_DECIMAL_DOUBLE,
    T_OPERAND_HEXDECIMAL_INTEGER,
    T_OPERATOR_SIGN_PLUS,
    T_OPERATOR_SIGN_MINUS,
    T_OPERATOR_LOGIC_NOT,
    T_OPERATOR_RELAT_NE,
    T_OPERATOR_ARITH_MOD,
    T_OPERATOR_ARITH_MODNASS,
    T_OPERATOR_BIT_AND,
    T_OPERATOR_LOGIC_AND,
    T_OPERATOR_BIT_ANDNASS,
    T_SYMBOL_SIMPLE_PARANO,
    T_SYMBOL_SIMPLE_PARANC,
    T_OPERATOR_ARITH_MULT,
    T_OPERATOR_ARITH_EXP,
    T_OPERATOR_ARITH_EXPNASS,
    T_OPERATOR_ARITH_MULTNASS,
    T_OPERATOR_ARITH_ADD,
    T_OPERATOR_ARITH_ADDNASS,
    T_SYMBOL_SIMPLE_COMMA,
    T_OPERATOR_ARITH_SUB,
    T_OPERATOR_ARITH_SUBNASS,
    T_SYMBOL_SHARP_ARROW,
    T_SYMBOL_SIMPLE_DOT,
    T_OPERATOR_SPECIAL_DDOT,
    T_SYMBOL_ELLIPSIS,
    T_OPERATOR_ARITH_DIV,
    T_OPERATOR_ARITH_DIVNASS,
    T_OPERATOR_ITER_OF,
    T_SYMBOL_TYPE_ALIAS,
    T_OPERATOR_TYPE_INFASS,
    T_SYMBOL_SIMPLE_SEMICOLON,
    T_OPERATOR_RELAT_LT,
    T_OPERATOR_BIT_LS,
    T_OPERATOR_BIT_LSNASS,
    T_OPERATOR_RELAT_LE,
    T_OPERATOR_SIMPLE_ASSIGN,
    T_OPERATOR_RELAT_EQ,
    T_OPERATOR_RELAT_GT,
    T_OPERATOR_RELAT_GE,
    T_OPERATOR_BIT_RS,
    T_OPERATOR_BIT_RSNASS,
    T_SYMBOL_NULLABLE_VAR,
    T_OPERATOR_TERN_COLEASCE,
    T_SYMBOL_ANONYMOUS_FUNCTION,
    T_SYMBOL_SIMPLE_SQRBRO,
    T_SYMBOL_SIMPLE_SQRBRC,
    T_OPERATOR_BIT_XOR,
    T_OPERATOR_BIT_XORNASS,
    T_SYMBOL_SIMPLE_CURLBRO,
    T_OPERATOR_BIT_OR,
    T_OPERATOR_BIT_ORNASS,
    T_OPERATOR_LOGIC_OR,
    T_SYMBOL_SIMPLE_CURLBRC,
    T_OPERATOR_BIT_NOT,
    T_RESERVED_KEYWORD_TYPE_BOOL,
    T_RESERVED_KEYWORD_STATEMENT_BREAK,
    T_RESERVED_KEYWORD_TYPE_CHAR,
    T_RESERVED_KEYWORD_STATEMENT_CONTINUE,
    T_RESERVED_KEYWORD_STATMENT_ELSE,
    T_RESERVED_KEYWORD_STATMENT_ENUM,
    T_RESERVED_KEYWORD_TYPE_F128,
    T_RESERVED_KEYWORD_TYPE_F32,
    T_RESERVED_KEYWORD_TYPE_F64,
    T_RESERVED_KEYWORD_CONSTANT_FALSE,
    T_RESERVED_KEYWORD_CONSTANT_FINALLY,
    T_RESERVED_KEYWORD_STATEMENT_FOR,
    T_RESERVED_KEYWORD_STATEMENT_FUNC,
    T_RESERVED_KEYWORD_TYPE_I16,
    T_RESERVED_KEYWORD_TYPE_I32,
    T_RESERVED_KEYWORD_TYPE_I64,
    T_RESERVED_KEYWORD_TYPE_I8,
    T_RESERVED_KEYWORD_STATEMENT_IF,
    T_RESERVED_KEYWORD_STATEMENT_MATCH,
    T_RESERVED_KEYWORD_STATEMENT_MIXIN,
    T_RESERVED_KEYWORD_STATEMENT_NEW,
    T_RESERVED_KEYWORD_CONSTANT_NULL,
    T_RESERVED_KEYWORD_STATEMENT_RAISE,
    T_RESERVED_KEYWORD_STATEMENT_REPEAT,
    T_RESERVED_KEYWORD_STATEMENT_RESCUE,
    T_RESERVED_KEYWORD_STATEMENT_RETURN,
    T_RESERVED_KEYWORD_TYPE_STRING,
    T_RESERVED_KEYWORD_STATEMENT_STRUCT,
    T_RESERVED_KEYWORD_STATEMENT_TEMPLATE,
    T_RESERVED_KEYWORD_CONSTANT_TRUE,
    T_RESERVED_KEYWORD_STATEMENT_TYPEDEF,
    T_RESERVED_KEYWORD_TYPE_U16,
    T_RESERVED_KEYWORD_TYPE_U32,
    T_RESERVED_KEYWORD_TYPE_U64,
    T_RESERVED_KEYWORD_TYPE_U8,
    T_RESERVED_KEYWORD_STATEMENT_UNTIL,
    T_RESERVED_KEYWORD_STATEMENT_WHEN,
    T_RESERVED_KEYWORD_STATEMENT_WHILE
}; // enum TOKENTYPE_E


struct token_pos_t {
    size_t line_no;
    size_t col_no;
};

struct token_t {
    enum TOKENTYPE_E token_type;
    char* token_buffer;
    struct token_pos_t pos;
    size_t line_start_pos;
    struct token_t* next_token;
}; // struct token_t


#define RESERVED_KEYWORD_OFFSET ((long int) T_RESERVED_KEYWORD_TYPE_BOOL)
#define SYMBOL_TOKEN_OFFSET ((long int) T_OPERATOR_LOGIC_NOT)
#define LAST_RESERVED_TOKEN T_RESERVED_KEYWORD_STATEMENT_WHILE
#define ESCAPE_CHARACTER ((char) 27)
#define CANCEL_CHARACTER ((char) 24)

#define RESERVED_KEYWORDS { \
    "bool", "break", "char", "continue", \
    "else", "enum", "f128", "f32", "f64", \
    "false", "finally", "for", "func", \
    "i16", "i32", "i64", "i8", "if", "match", \
    "mixin", "new", "null", "raise", "repeat", \
    "rescue", "return", "string", "struct", \
    "template", "true", "typedef", "u16", "u32", \
    "u64", "u8", "until", "when", "while" \
}

#define VALID_SYMBOLS { \
    "!", "!=", "%", "%=", "&", \
    "&&", "&=", "(", ")", "*", \
    "**", "**=", "*=", "+", \
    "+=", ",", "-", "-=", "->", \
    ".", "..", "...", "/", "/=", \
    ":", "::", ":=", ";", "<", \
    "<<", "<<=", "<=", "=", "==", \
    ">", ">=", ">>", ">>=", "?", \
    "??", "@", "[", "]", "^", "^=", \
    "{", "|", "|=", "||", "}", "~" \
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

const char* stringify_token(enum TOKENTYPE_E token_type);
bool is_datatype_token(enum TOKENTYPE_E token_type);
bool is_bracket_token(enum TOKENTYPE_E token_type);
bool is_possible_operand(enum TOKENTYPE_E token_type);
bool is_unary_operator(enum TOKENTYPE_E token_type);
bool is_binary_operator(enum TOKENTYPE_E token_type);
bool is_assignment_operator(enum TOKENTYPE_E token_type);
bool not_allowed_after_operator(enum TOKENTYPE_E token_type);
bool is_allowed_before_datatype(enum TOKENTYPE_E token_type);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __TOKEN_H__