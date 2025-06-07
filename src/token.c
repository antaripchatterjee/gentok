#ifdef MAKEFILECOMPILING
#include "token.h"
#else
#include "../include/token.h"
#endif // MAKEFILECOMPILING


bool is_statement_token(enum TOKENTYPE_E token_type) {
    return false;
}

bool does_statement_expect_expression(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_datatype_token(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_bracket_token(enum TOKENTYPE_E token_type) {
    return token_type == T_SYMBOL_SIMPLE_CURLBRO
        || token_type == T_SYMBOL_SIMPLE_SQRBRO
        || token_type == T_SYMBOL_SIMPLE_CURLBRC
        || token_type == T_SYMBOL_SIMPLE_SQRBRC;
}

bool is_possible_operand(enum TOKENTYPE_E token_type, int prev_or_current) {
    return token_type == T_OPERAND_STRING_VALUE
        || token_type == T_OPERAND_DECIMAL_DOUBLE
        || token_type == T_OPERAND_DECIMAL_INTEGER
        || (prev_or_current < 0 &&  token_type == T_SYMBOL_SIMPLE_SQRBRC)
        || (prev_or_current >= 0 &&  token_type == T_SYMBOL_SIMPLE_SQRBRO);
}

bool is_sign_token(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_unary_operator(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_binary_operator(enum TOKENTYPE_E token_type) {
    // TODO: implement logic to find if token_type is binary operator
    return false;
}

bool is_assignment_operator(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_allowed_with_binary_operator(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_allowed_with_unary_operator(enum TOKENTYPE_E token_type) {
    return false;
}

bool is_allowed_before_datatype(enum TOKENTYPE_E token_type) {
    return false;
}