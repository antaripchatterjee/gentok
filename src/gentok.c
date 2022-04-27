#ifdef MAKEFILECOMPILING
#include "gentok.h"
#else
#include "../include/gentok.h"
#endif // MAKEFILECOMPILING

static char* append_character(char* token, char ch) {
    size_t next_size = strlen(token) + 2;
    token = (char*) realloc(token, sizeof(char) * next_size);
    token[next_size-2] = ch;
    token[next_size-1] = '\0';
    return token;
}

static struct token_t* add_token(struct token_t* token, const char* token_buffer, enum TOKENTYPE_E token_type) {
    token->token_type = token_type;
    token->token_buffer = (char*) malloc(sizeof(char) * (strlen(token_buffer) + 1));
    memset(token->token_buffer, '\0', sizeof(char) * (strlen(token_buffer) + 1));
    strcpy(token->token_buffer, token_buffer);
    token->next_token = (struct token_t*) malloc(sizeof(struct token_t));
    token->next_token->token_type = T_TOKEN_UNKNOWN;
    token->next_token->token_buffer = NULL;
    token->next_token->next_token = NULL;
    return token->next_token;
}

static bool treat_as_sign(enum TOKENTYPE_E prev_token_type) {
    return prev_token_type != T_OPERAND_IDENTIFIER &&
        prev_token_type != T_OPERAND_STRING_VALUE &&
        prev_token_type != T_OPERAND_BINARY_NUMBER &&
        prev_token_type != T_OPERAND_OCTAL_NUMBER &&
        prev_token_type != T_OPERAND_DECIMAL_NUMBER &&
        prev_token_type != T_OPERAND_HEXDECIMAL_NUMBER &&
        prev_token_type != T_SYMBOL_CLOSING_CURLY_BRACKET &&
        prev_token_type != T_SYMBOL_CLOSING_PARANANTHESIS &&
        prev_token_type != T_SYMBOL_CLOSING_SQUARE_BRACKET;
}

bool tokenize(const char* script, struct token_t* token, long* number_of_tokens) {
    const char* reserved_keywords[] = RESERVED_KEYWORDS;
    const char* other_valid_tokens[] = OTHER_VALID_TOKENS;

    const long count_reserved_keywords = sizeof(reserved_keywords)/sizeof(char*);
    const long count_other_tokens = sizeof(other_valid_tokens)/sizeof(char*);

    char curr_char, prev_char = '\0';
    char string_character = '\0';

    bool success = true;

    bool is_word = false, is_number = false, is_comment = false,
        is_string = false, is_symbol_or_operator = false, is_sign = false;
    short number_base_system = 0;
    bool exp_appended = false, decimal_appended = false, allow_char_as_num = false;
    bool reset_current_token = false;
    
    enum TOKENTYPE_E current_token_type = T_TOKEN_UNKNOWN, prev_token_type;
    struct token_t* current_token = token;
    char* token_buffer = (char*) malloc(sizeof(char) * 1);
    token_buffer[0] = '\0';

    unsigned long line_no = 1L, col_no = 1L;

    for(size_t index = 0; (curr_char = script[index]) != '\0' && success; index++) {
        if((curr_char == '\n' && prev_char != '\r') || curr_char == '\r') {
            line_no++;
            col_no = 1L;
        } else if(curr_char != '\n') {
            col_no++;
        }
        if(!is_comment) {
            if(!isspace(curr_char) && !isprint(curr_char)) {
                raise_invalid_character_error(index, line_no, col_no,
                    "Found '%c'(ASCII %d)\n", curr_char, (int) curr_char);
                success = false;
            }
            if(is_string) {
                token_buffer = append_character(token_buffer, curr_char);
                if(prev_char != '\\') {
                    if(curr_char == string_character) {
                        current_token_type = T_OPERAND_STRING_VALUE;
                        reset_current_token = true;
                        index++;
                        is_string = false;
                    }
                }
            } else {
                if(is_word) {
                    if(isalnum(curr_char) || curr_char == '_') {
                        token_buffer = append_character(token_buffer, curr_char);
                    } else {
                        long token_index = get_index_from(token_buffer, count_reserved_keywords, reserved_keywords);
                        if(token_index == -1) {
                            current_token_type = T_OPERAND_IDENTIFIER;
                        } else {
                            current_token_type = (enum TOKENTYPE_E) (token_index + RESERVED_KEYWORD_OFFSET);
                        }
                        is_word = false;
                        reset_current_token = true;
                    }
                } else if(is_number) {
                    if(number_base_system == 0) {
                        if(prev_char == '0') {
                            if(curr_char == '.' || tolower(curr_char) == 'e') {
                                number_base_system = 10;
                                decimal_appended = true;
                                if(curr_char == 'e') {
                                    exp_appended = true;
                                }
                            } else if (tolower(curr_char) == 'b') {
                                number_base_system = 2;
                            } else if(tolower(curr_char) == 'o') {
                                number_base_system = 8;
                            } else if(tolower(curr_char) == 'x') {
                                number_base_system = 16;
                            } else if(curr_char >= '0' && curr_char < '8') {
                                token_buffer = append_character(token_buffer, 'o');
                                number_base_system = 8;
                            } else if(isalnum(curr_char) || curr_char == '_') {
                                // Raise invalid syntax error
                                allow_char_as_num = false;
                                raise_invalid_token_error(
                                    line_no, col_no, index,
                                    "Found -> \"%s\", not a valid number. %s.\n", token_buffer,
                                    "\"8-9\", `_`, \"a-z\" or \"A-Z\" are not expected"
                                );
                                success = false;
                            } else {
                                number_base_system = 10;
                                allow_char_as_num = false;
                            }
                        } else {
                            number_base_system = 10;
                            if(isdigit(curr_char) || curr_char == '.' || tolower(curr_char) == 'e') {
                                if(curr_char == '.') {
                                    decimal_appended = true;
                                } else if(tolower(curr_char) == 'e') {
                                    decimal_appended = true;
                                    exp_appended = true;
                                }
                            } else if(isalpha(curr_char) || curr_char == '_') {
                                // Raise invalid syntax error
                                allow_char_as_num = false;
                                raise_invalid_token_error(
                                    line_no, col_no, index,
                                    "Found -> \"%s\", not a valid decimal number. %s.\n",
                                    token_buffer, "`_`, \"a-z\", \"A-Z\" are not expected"
                                );
                                success = false;
                            } else {
                                allow_char_as_num = false;
                            }
                        }
                    } else if(number_base_system == 2) {
                        if(curr_char != '0' || curr_char != '1') {
                            allow_char_as_num = false;
                            if(isalnum(curr_char) || curr_char == '_' || curr_char == '.' || tolower(prev_char) == 'b') {
                                // Raise invalid syntax error
                                raise_invalid_token_error(
                                    line_no, col_no, index,
                                    "Found -> \"%s\", not a valid binary number. %s.\n", token_buffer,
                                    tolower(prev_char) == 'b' ? "\"0-1\" are expected"
                                        : "\"2-9\", `.`, `_`, \"a-z\", \"A-Z\" are unexpected"
                                );
                                success = false;
                            }
                        } 
                    } else if(number_base_system == 8) {
                        if(curr_char < '0' || curr_char > '7') {
                            allow_char_as_num = false;
                            if(isalnum(curr_char) || curr_char == '_' || curr_char == '.' || tolower(prev_char) == 'o') {
                                // Raise invalid syntax error
                                raise_invalid_token_error(
                                    line_no, col_no, index,
                                    "Found -> \"%s\", not a valid octal number. %s.\n", token_buffer,
                                    tolower(prev_char) == 'o' ? "\"0-7\" are expected"
                                        : "\"8-9\", `.`, `_`, \"a-z\", \"A-Z\" are unexpected"
                                );
                                success = false;
                            }
                        } 
                    } else if(number_base_system == 16) {
                        if(!isxdigit(curr_char)){
                            allow_char_as_num = false;
                            if(isalpha(curr_char) || curr_char == '_' || curr_char == '.' || tolower(prev_char) == 'x') {
                                // Raise invalid syntax error
                                raise_invalid_token_error(
                                    line_no, col_no, index,
                                    "Found -> \"%s\", not a valid hexadecimal number. %s.\n", token_buffer,
                                    tolower(prev_char) == 'x' ? "\"0-9\", \"a-f\", \"A-F\" are expected"
                                        : "`.`, `_`, \"g-z\", \"G-Z\" are unexpected"
                                );
                                success = false;
                            }
                        } 
                    } else if(number_base_system == 10) {
                        if(isdigit(curr_char) || (curr_char == '.' && !decimal_appended)
                            || (tolower(curr_char) == 'e' && !exp_appended)
                            || ((curr_char == '+' || curr_char == '-') && tolower(prev_char) == 'e' && exp_appended)) {
                            if(curr_char == '.') {
                                decimal_appended = true;
                            } else if(tolower(curr_char) == 'e') {
                                decimal_appended = true;
                                exp_appended = true;
                            }
                        } else if(isalpha(curr_char) || curr_char == '_' || curr_char == '.' || ((prev_char == '+' || prev_char == '-') && exp_appended)) {
                            // Raise the invalid syntax error
                            allow_char_as_num = false;
                            raise_invalid_token_error(
                                line_no, col_no, index,
                                "Found -> \"%s\", not a valid decimal number. %s.\n", token_buffer,
                                (prev_char == '+' || prev_char == '-') ? "\"0-9\" are expected"
                                    : "`.`, `_`, \"g-z\", \"G-Z\" are unexpected"
                            );
                            success = false;
                        } else {
                            allow_char_as_num = false;
                        }
                    } else {
                        //  Raise invalid number base error
                        allow_char_as_num = false;
                        raise_invalid_token_error(
                            line_no, col_no, index,
                            "Found -> \"%s\", could not identify the base of the number.\n",
                            token_buffer
                        );
                        success = false;
                    }
                    if(allow_char_as_num) {
                        token_buffer = append_character(token_buffer, tolower(curr_char));
                    } else {
                        current_token_type = number_base_system == 2 ? T_OPERAND_BINARY_NUMBER
                            : number_base_system == 8 ? T_OPERAND_OCTAL_NUMBER
                            : number_base_system == 10 ? T_OPERAND_DECIMAL_NUMBER
                            : number_base_system == 16 ? T_OPERAND_HEXDECIMAL_NUMBER
                            : T_TOKEN_INVALID;
                        reset_current_token = true;
                        is_number = false;
                        number_base_system = 0;
                        exp_appended = decimal_appended = allow_char_as_num = false;
                    }
                } else if(is_sign) {
                    if(isspace(curr_char)) {
                        continue;
                    } else if(isalnum(curr_char) || curr_char == '_' || curr_char == '(' || curr_char == '[') {
                        // add the token as unary operator and reset the token buffer
                        current_token_type = token_buffer[strlen(token_buffer) - 1] == '+'
                            ? T_OPERATOR_UNARY_SIGN_PLUS : T_OPERATOR_UNARY_SIGN_MINUS;
                        is_sign = false;
                        reset_current_token = true;
                    } else if(curr_char == '+' || curr_char == '-') {
                        // update the unary sign operator
                        token_buffer[strlen(token_buffer) - 1] = token_buffer[strlen(token_buffer) - 1] == curr_char ? '+': '-';
                    } else {
                        // Raise syntax error
                        is_sign = false;
                        raise_invalid_token_error(
                            line_no, col_no, index,
                            "Found -> \"%s\", not a valid unary sign operator. %s.\n",
                            token_buffer, "expected \"a-z\", \"0-9\", `_`, `+` or `-`"
                        );
                        success = false;
                    }
                } else if(is_symbol_or_operator) {
                    token_buffer = append_character(token_buffer, curr_char);
                    long token_index = get_index_from(token_buffer, count_other_tokens, other_valid_tokens);
                    if(token_index == -1) {
                        token_buffer[strlen(token_buffer)-1] = '\0';
                        reset_current_token = true;
                        is_symbol_or_operator = false;
                    } else {
                        current_token_type = (enum TOKENTYPE_E) (token_index + OTHER_TOKEN_OFFSET);
                    }
                } else {
                    if(isalpha(curr_char) || curr_char == '_') {
                        token_buffer = append_character(token_buffer, curr_char);
                        is_word = true;
                    } else if(isdigit(curr_char)) {
                        token_buffer = append_character(token_buffer, curr_char);
                        is_number = true;
                        allow_char_as_num = true;
                    } else if(curr_char == '#') {
                        is_comment = true;
                    } else if(curr_char == '\'' || curr_char == '"') {
                        token_buffer = append_character(token_buffer, curr_char);
                        string_character = curr_char;
                        is_string = true;
                    } else if(!isspace(curr_char)) {
                        token_buffer = append_character(token_buffer, curr_char);
                        long token_index = get_index_from(token_buffer, count_other_tokens, other_valid_tokens);
                        if(token_index != -1) {
                            if((curr_char == '+' || curr_char == '-') && treat_as_sign(prev_token_type)) {
                                is_sign = true;
                            } else {
                                current_token_type = (enum TOKENTYPE_E) (token_index + OTHER_TOKEN_OFFSET);
                                is_symbol_or_operator = true;
                            }
                        } else {
                            // Raise invalid token error
                            raise_invalid_token_error(
                                line_no, col_no, index,
                                "Found -> \"%s\", not a valid operator or symbol\n", token_buffer
                            );
                            success = false;
                        }
                    }
                }
            }
        } else {
            if((curr_char == '\n' && prev_char != '\r') || curr_char == '\r') {
                is_comment = false;
            }
        }
        if(reset_current_token) {
            current_token = add_token(current_token, (const char*) token_buffer, current_token_type);
            (*number_of_tokens)++;
            token_buffer = (char*) realloc(token_buffer, sizeof(char) * 1);
            token_buffer[0] = '\0';
            index--;
            reset_current_token = false;
            prev_token_type = current_token_type;
            current_token_type = T_TOKEN_UNKNOWN;
            prev_char = '\0';
        } else {
            prev_char = curr_char;
        }
    }
    free(token_buffer);
    return success;
}

long free_tokens(struct token_t* token) {
    struct token_t* _next_token = token;
    long number_of_tokens = 0;
    while(_next_token) {
        struct token_t* _temp_token = _next_token;
        _next_token = _next_token->next_token;
        if(_temp_token->token_buffer) {
            free(_temp_token->token_buffer);
            number_of_tokens++;
        }
        if(_temp_token != token) free(_temp_token);
    }
    return number_of_tokens;
}