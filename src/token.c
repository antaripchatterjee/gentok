#ifdef MAKEFILECOMPILING
#include "token.h"
#else
#include "../include/token.h"
#endif // MAKEFILECOMPILING

const char* stringify_token(enum TOKENTYPE_E token_type) {
    const char* reserved_keywords[] = RESERVED_KEYWORDS;
    const char* valid_symbols[] = VALID_SYMBOLS;

    return token_type == T_OPERATOR_SIGN_PLUS ? "+"
        : token_type == T_OPERATOR_SIGN_MINUS ? "-"
        : token_type > LAST_RESERVED_TOKEN ? NULL
        : token_type >= RESERVED_KEYWORD_OFFSET ? reserved_keywords[(size_t)(token_type-RESERVED_KEYWORD_OFFSET)]
        : token_type >= SYMBOL_TOKEN_OFFSET ? valid_symbols[(size_t)(token_type-SYMBOL_TOKEN_OFFSET)]
        : NULL;
}

bool is_datatype_token(enum TOKENTYPE_E token_type) {
    return token_type == T_RESERVED_KEYWORD_TYPE_BOOL
        || token_type == T_RESERVED_KEYWORD_TYPE_CHAR
        || token_type == T_RESERVED_KEYWORD_TYPE_F128
        || token_type == T_RESERVED_KEYWORD_TYPE_F32
        || token_type == T_RESERVED_KEYWORD_TYPE_F64
        || token_type == T_RESERVED_KEYWORD_TYPE_I16
        || token_type == T_RESERVED_KEYWORD_TYPE_I32
        || token_type == T_RESERVED_KEYWORD_TYPE_I64
        || token_type == T_RESERVED_KEYWORD_TYPE_I8
        || token_type == T_RESERVED_KEYWORD_TYPE_STRING
        || token_type == T_RESERVED_KEYWORD_TYPE_U16
        || token_type == T_RESERVED_KEYWORD_TYPE_U32
        || token_type == T_RESERVED_KEYWORD_TYPE_U64
        || token_type == T_RESERVED_KEYWORD_TYPE_U8;
}

bool is_bracket_token(enum TOKENTYPE_E token_type) {
    return token_type == T_SYMBOL_SIMPLE_PARANO
        || token_type == T_SYMBOL_SIMPLE_CURLBRO
        || token_type == T_SYMBOL_SIMPLE_SQRBRO
        || token_type == T_SYMBOL_SIMPLE_PARANC
        || token_type == T_SYMBOL_SIMPLE_CURLBRC
        || token_type == T_SYMBOL_SIMPLE_SQRBRC;
}

bool is_possible_operand(enum TOKENTYPE_E token_type) {
    return token_type == T_OPERAND_IDENTIFIER ||
        token_type == T_OPERAND_STRING_VALUE ||
        token_type == T_OPERAND_STRING_TEMPLATE ||
        token_type == T_OPERAND_BINARY_INTEGER ||
        token_type == T_OPERAND_OCTAL_INTEGER ||
        token_type == T_OPERAND_DECIMAL_DOUBLE ||
        token_type == T_OPERAND_DECIMAL_INTEGER ||
        token_type == T_OPERAND_HEXDECIMAL_INTEGER ||
        token_type == T_SYMBOL_SIMPLE_PARANC ||
        token_type == T_SYMBOL_SIMPLE_SQRBRC;
}

bool is_unary_operator(enum TOKENTYPE_E token_type) {
    return token_type == T_OPERATOR_LOGIC_NOT
        || token_type == T_OPERATOR_SIGN_PLUS
        || token_type == T_OPERATOR_SIGN_MINUS;
}

bool is_binary_operator(enum TOKENTYPE_E token_type) {
    // TODO: implement logic to find if token_type is binary operator
    return token_type == T_OPERATOR_RELAT_NE
        || token_type == T_OPERATOR_ARITH_MOD
        || token_type == T_OPERATOR_ARITH_MODNASS
        || token_type == T_OPERATOR_BIT_AND
        || token_type == T_OPERATOR_LOGIC_AND
        || token_type == T_OPERATOR_BIT_ANDNASS
        || token_type == T_OPERATOR_ARITH_MULT
        || token_type == T_OPERATOR_ARITH_EXP
        || token_type == T_OPERATOR_ARITH_EXPNASS
        || token_type == T_OPERATOR_ARITH_MULTNASS
        || token_type == T_OPERATOR_ARITH_ADD
        || token_type == T_OPERATOR_ARITH_ADDNASS
        || token_type == T_OPERATOR_ARITH_SUB
        || token_type == T_OPERATOR_ARITH_SUBNASS
        || token_type == T_OPERATOR_ARITH_DIV
        || token_type == T_OPERATOR_ARITH_DIVNASS
        || token_type == T_OPERATOR_TYPE_INFASS
        || token_type == T_OPERATOR_RELAT_LT
        || token_type == T_OPERATOR_BIT_LS
        || token_type == T_OPERATOR_BIT_LSNASS
        || token_type == T_OPERATOR_RELAT_LE
        || token_type == T_OPERATOR_SIMPLE_ASSIGN
        || token_type == T_OPERATOR_RELAT_EQ
        || token_type == T_OPERATOR_RELAT_GT
        || token_type == T_OPERATOR_RELAT_GE
        || token_type == T_OPERATOR_BIT_RS
        || token_type == T_OPERATOR_BIT_RSNASS
        || token_type == T_OPERATOR_TERN_COLEASCE
        || token_type == T_OPERATOR_BIT_XOR
        || token_type == T_OPERATOR_BIT_XORNASS
        || token_type == T_OPERATOR_BIT_OR
        || token_type == T_OPERATOR_BIT_ORNASS
        || token_type == T_OPERATOR_LOGIC_OR
        || token_type == T_OPERATOR_BIT_NOT;
}

bool is_assignment_operator(enum TOKENTYPE_E token_type) {
    return token_type == T_OPERATOR_TYPE_INFASS
        || token_type == T_OPERATOR_SIMPLE_ASSIGN
        || token_type == T_OPERATOR_ARITH_MODNASS
        || token_type == T_OPERATOR_BIT_ANDNASS
        || token_type == T_OPERATOR_ARITH_EXPNASS
        || token_type == T_OPERATOR_ARITH_MULTNASS
        || token_type == T_OPERATOR_ARITH_ADDNASS
        || token_type == T_OPERATOR_ARITH_SUBNASS
        || token_type == T_OPERATOR_ARITH_DIVNASS
        || token_type == T_OPERATOR_BIT_LSNASS
        || token_type == T_OPERATOR_BIT_RSNASS
        || token_type == T_OPERATOR_BIT_XORNASS
        || token_type == T_OPERATOR_BIT_ORNASS;
}

bool not_allowed_after_operator(enum TOKENTYPE_E token_type) {
    // TODO: implement logic to find if token_type is not allowed after operator
    return false;
}

bool is_allowed_before_datatype(enum TOKENTYPE_E token_type) {
    return token_type == T_OPERAND_IDENTIFIER
        || token_type == T_SYMBOL_TYPE_ALIAS
        || token_type == T_SYMBOL_ELLIPSIS
        || token_type == T_SYMBOL_SIMPLE_PARANO
        || token_type == T_SYMBOL_SIMPLE_PARANC;
}