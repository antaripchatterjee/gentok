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
    T_OPERAND_STRING_VALUE,
    T_OPERAND_DECIMAL_INTEGER,
    T_OPERAND_DECIMAL_DOUBLE,
    T_SYMBOL_SIMPLE_COMMA,
    T_SYMBOL_SIMPLE_COLON,
    T_SYMBOL_SIMPLE_SQRBRO,
    T_SYMBOL_SIMPLE_SQRBRC,
    T_SYMBOL_SIMPLE_CURLBRO,
    T_SYMBOL_SIMPLE_CURLBRC,
    T_RESERVED_KEYWORD_CONSTANT_FALSE,
    T_RESERVED_KEYWORD_CONSTANT_NULL,
    T_RESERVED_KEYWORD_CONSTANT_TRUE
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


#define RESERVED_KEYWORD_OFFSET ((long int) T_RESERVED_KEYWORD_CONSTANT_FALSE)
#define SYMBOL_TOKEN_OFFSET ((long int) T_SYMBOL_SIMPLE_COMMA)
#define LAST_RESERVED_TOKEN T_RESERVED_KEYWORD_CONSTANT_TRUE
#define ESCAPE_CHARACTER ((char) 27)
#define CANCEL_CHARACTER ((char) 24)

#define RESERVED_KEYWORDS { \
    "false", "null", "true" \
}

#define VALID_SYMBOLS { \
    ',', ':', '[', ']', '{', '}' \
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool is_statement_token(enum TOKENTYPE_E token_type);
bool is_datatype_token(enum TOKENTYPE_E token_type);
bool is_bracket_token(enum TOKENTYPE_E token_type);
bool is_possible_operand(enum TOKENTYPE_E token_type, int prev_or_current);
bool is_sign_token(enum TOKENTYPE_E token_type);
bool is_unary_operator(enum TOKENTYPE_E token_type);
bool is_binary_operator(enum TOKENTYPE_E token_type);
bool is_assignment_operator(enum TOKENTYPE_E token_type);
bool is_allowed_with_binary_operator(enum TOKENTYPE_E token_type);
bool is_allowed_with_unary_operator(enum TOKENTYPE_E token_type);
bool is_allowed_before_datatype(enum TOKENTYPE_E token_type);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __TOKEN_H__