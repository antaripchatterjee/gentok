#ifdef MAKEFILECOMPILING
#include "gentok.h"
#else
#include "../include/gentok.h"
#endif // MAKEFILECOMPILING

#include <stdio.h>

static struct token_t* add_token(struct token_t* token, const char* token_buffer, enum TOKENTYPE_E token_type, size_t line_no, size_t col_no, size_t line_start_pos) {
    token->token_type = token_type;
    token->token_buffer = (char*) malloc(sizeof(char) * (strlen(token_buffer) + 1));
    memset(token->token_buffer, '\0', sizeof(char) * (strlen(token_buffer) + 1));
    strcpy(token->token_buffer, token_buffer);
    token->pos = (struct token_pos_t) { .line_no = line_no, .col_no = col_no};
    token->line_start_pos = line_start_pos;
    token->next_token = (struct token_t*) malloc(sizeof(struct token_t));
    token->next_token->token_type = T_TOKEN_UNKNOWN;
    token->next_token->token_buffer = NULL;
    token->next_token->next_token = NULL;
    return token->next_token;
}

static bool treat_symbol_as_operator(enum TOKENTYPE_E prev_token_type) {
    return prev_token_type == T_OPERAND_IDENTIFIER ||
        prev_token_type == T_OPERAND_STRING_VALUE ||
        prev_token_type == T_OPERAND_STRING_TEMPLATE ||
        prev_token_type == T_OPERAND_BINARY_INTEGER ||
        prev_token_type == T_OPERAND_OCTAL_INTEGER ||
        prev_token_type == T_OPERAND_DECIMAL_DOUBLE ||
        prev_token_type == T_OPERAND_DECIMAL_INTEGER ||
        prev_token_type == T_OPERAND_HEXDECIMAL_INTEGER ||
        prev_token_type == T_SYMBOL_SIMPLE_PARANC ||
        prev_token_type == T_SYMBOL_SIMPLE_SQRBRC;
}

bool tokenize(const char* script, struct token_t* token, long* number_of_tokens) {
    const char* reserved_keywords[] = RESERVED_KEYWORDS;
    const char* valid_symbols[] = VALID_SYMBOLS;

    const long count_reserved_keywords = sizeof(reserved_keywords)/sizeof(char*);
    const long count_valid_symbols = sizeof(valid_symbols)/sizeof(char*);

    char curr_char, prev_char = '\0';
    char string_character = '\0';

    bool success = true;

    bool is_word = false, is_number = false, is_comment = false,
        is_string = false, is_symbol = false, is_new_line = false,
        is_sign = false, is_template_string = false;
    short number_base_system = 0;
    bool exp_appended = false, decimal_appended = false, allow_char_as_num = false;
    bool reset_current_token = false;
    
    enum TOKENTYPE_E current_token_type = T_TOKEN_UNKNOWN, prev_token_type;
    struct token_t* current_token = token;
    char* token_buffer = (char*) malloc(sizeof(char) * 1);
    token_buffer[0] = '\0';

    size_t line_no = 1, col_no = 1, line_start_pos = 0;
    size_t token_line_no, token_col_no;
    struct token_pos_t string_pos = { 0UL, 0UL };
    char esc_seq_str[ESCAPE_SEQ_MAX_LEN] = { 0 };
    esc_seq_validator_t escape_seq_validator = NULL;
    int escape_seq_char_count = -1;
    for(size_t index = 0; (curr_char = script[index]) != '\0' && success; col_no++, index++) {
        if((prev_char == '\r' || prev_char == '\n') && !is_string) {
            line_start_pos = index;
        }
        if(!is_comment) {
            if(!isspace(curr_char) && !isprint(curr_char)) {
                raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                    "Found invalid character '%c'(ASCII %d)\n", curr_char, (int) curr_char);
                success = false;
            }
            if(is_string) {
                if((curr_char == '\n' || curr_char == '\r') && !is_template_string) {
                    raise_error(script, line_start_pos, "INVALID TOKEN", string_pos.line_no, string_pos.col_no, index, 
                        "Invalid string literal, missing string ending character %c", string_character);
                    success = false;
                }
                if(success) {
                    token_buffer = append_character(token_buffer, curr_char);
                    if(prev_char == ESCAPE_CHARACTER) {
                        char esc_seq_err[ESCAPE_SEQ_ERR_MSG_SZ] = { 0 };
                        if(escape_seq_validator && escape_seq_char_count > 0) {
                            if(!escape_seq_validator(curr_char, &escape_seq_char_count, esc_seq_str, esc_seq_err)) {
                                // error
                                raise_error(script, line_start_pos, "INVALID TOKEN", string_pos.line_no, string_pos.col_no, index, esc_seq_err);
                                memset(esc_seq_err, 0, ESCAPE_SEQ_ERR_MSG_SZ);
                                success = false;
                            }
                        } else if(!escape_seq_validator) {
                            escape_seq_char_count = get_esc_seq_validation_rule(curr_char, &escape_seq_validator);
                            if(escape_seq_char_count < 0) {
                                raise_error(script, line_start_pos, "INVALID TOKEN", string_pos.line_no, string_pos.col_no, index,
                                    "Invalid string literal, unknown escape sequence '\\%c'", curr_char);
                                success = false;
                            } else if(escape_seq_validator == &octal_seq_validator) {
                                esc_seq_str[0] = curr_char;
                            }
                        }
                        if(!escape_seq_char_count && escape_seq_validator) {
                            memset(esc_seq_str, 0, ESCAPE_SEQ_MAX_LEN);
                            escape_seq_char_count = -1;
                            escape_seq_validator = NULL;
                            prev_char = '\0';
                        }
                    }
                    if(curr_char == '\\' && prev_char != ESCAPE_CHARACTER) {
                        curr_char = ESCAPE_CHARACTER;
                    } else if(curr_char == string_character && prev_char != ESCAPE_CHARACTER) {
                        current_token_type = is_template_string ? 
                            T_OPERAND_STRING_TEMPLATE : T_OPERAND_STRING_VALUE;
                        reset_current_token = true;
                        index++;
                        col_no++;
                        is_string = false;
                        is_template_string = false;
                    }
                    if(escape_seq_char_count > 0) {
                        curr_char = ESCAPE_CHARACTER;
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
                            } else if(tolower(curr_char) == 'o' || isodigit(curr_char)) {
                                number_base_system = 8;
                            } else if(tolower(curr_char) == 'x') {
                                number_base_system = 16;
                            } else if(isalnum(curr_char) || curr_char == '_') {
                                // Raise invalid syntax error
                                allow_char_as_num = false;
                                raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                                    "Found '%c' (ASCII %d), invalid number sequence.\n", curr_char, curr_char
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
                                raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                                    "Found '%c' (ASCII %d), not a valid decimal digit.\n", curr_char, curr_char
                                );
                                success = false;
                            } else {
                                allow_char_as_num = false;
                            }
                        }
                    } else if(number_base_system == 2) {
                        if(!isbdigit(curr_char)) {
                            allow_char_as_num = false;
                            if(isalnum(curr_char) || curr_char == '_' || curr_char == '.' || tolower(prev_char) == 'b') {
                                // Raise invalid syntax error
                                raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                                    "Found '%c' (ASCII %d), not a valid binary digit.\n", curr_char, curr_char
                                );
                                success = false;
                            }
                        } 
                    } else if(number_base_system == 8) {
                        if(!isodigit(curr_char)) {
                            allow_char_as_num = false;
                            if(isalnum(curr_char) || curr_char == '_' || curr_char == '.' || tolower(prev_char) == 'o') {
                                // Raise invalid syntax error
                                raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                                    "Found '%c' (ASCII %d), not a valid octal digit.\n", curr_char, curr_char
                                );
                                success = false;
                            }
                        } 
                    } else if(number_base_system == 16) {
                        if(!isxdigit(curr_char)){
                            allow_char_as_num = false;
                            if(isalpha(curr_char) || curr_char == '_' || curr_char == '.' || tolower(prev_char) == 'x') {
                                // Raise invalid syntax error
                                raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                                    "Found '%c' (ASCII %d), not a valid hexadecimal digit.\n", curr_char, curr_char
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
                        } else if(isalpha(curr_char) || curr_char == '_' || curr_char == '.' || ((prev_char == '+' || prev_char == '-' || tolower(prev_char) == 'e') && exp_appended)) {
                            // Raise the invalid syntax error
                            allow_char_as_num = false;
                            raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                                "Found '%c', not a valid decimal digit.\n", curr_char
                            );
                            success = false;
                        } else {
                            allow_char_as_num = false;
                        }
                    } else {
                        //  Raise invalid number base error
                        allow_char_as_num = false;
                        raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                            "Found '%c', could not identify the base of the number.\n", curr_char
                        );
                        success = false;
                    }
                    if(allow_char_as_num) {
                        token_buffer = append_character(token_buffer, tolower(curr_char));
                    } else {
                        current_token_type = number_base_system == 2 ? T_OPERAND_BINARY_INTEGER
                            : number_base_system == 8 ? T_OPERAND_OCTAL_INTEGER
                            : number_base_system == 10 && (exp_appended || decimal_appended) ? T_OPERAND_DECIMAL_DOUBLE
                            : number_base_system == 10 ? T_OPERAND_DECIMAL_INTEGER
                            : number_base_system == 16 ? T_OPERAND_HEXDECIMAL_INTEGER
                            : T_TOKEN_INVALID;
                        reset_current_token = true;
                        is_number = false;
                        number_base_system = 0;
                        exp_appended = decimal_appended = allow_char_as_num = false;
                    }
                } else if(is_sign) {
                    if(isspace(curr_char)) {
                        continue;
                    } else if(isalnum(curr_char) || curr_char == '_') {
                        // add the token as unary operator and reset the token buffer
                        current_token_type = token_buffer[strlen(token_buffer) - 1] == '+'
                            ? T_OPERATOR_SIGN_PLUS : T_OPERATOR_SIGN_MINUS;
                        is_sign = false;
                        reset_current_token = true;
                    } else if(curr_char == '+' || curr_char == '-') {
                        // update the unary operator
                        token_buffer[strlen(token_buffer) - 1] = token_buffer[strlen(token_buffer) - 1] == curr_char ? '+': '-';
                    } else {
                        // Raise syntax error
                        is_sign = false;
                        raise_error(script, line_start_pos, "INVALID CHARACTER", line_no, col_no, index,
                            "Found '%c', invalid symbol after unary sign.\n", curr_char
                        );
                        success = false;
                    }
                } else if(is_symbol) {
                    token_buffer = append_character(token_buffer, curr_char);
                    long token_index = get_index_from(token_buffer, count_valid_symbols, valid_symbols);
                    if(token_index == -1) {
                        token_buffer[strlen(token_buffer)-1] = '\0';
                        reset_current_token = true;
                        is_symbol = false;
                    } else {
                        current_token_type = (enum TOKENTYPE_E) (token_index + SYMBOL_TOKEN_OFFSET);
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
                    } else if(curr_char == '\'' || curr_char == '"' || curr_char == '`') {
                        token_buffer = append_character(token_buffer, curr_char);
                        string_character = curr_char;
                        is_string = true;
                        if(curr_char == '`') is_template_string = true;
                        string_pos.line_no = line_no;
                        string_pos.col_no = col_no;
                    } else if(!isspace(curr_char)) {
                        token_buffer = append_character(token_buffer, curr_char);
                        long token_index = get_index_from(token_buffer, count_valid_symbols, valid_symbols);
                        if(token_index != -1) {
                            if((curr_char == '+' || curr_char == '-') && !treat_symbol_as_operator(prev_token_type)) {
                                is_sign = true;
                            } else {
                                current_token_type = (enum TOKENTYPE_E) (token_index + SYMBOL_TOKEN_OFFSET);
                                is_symbol = true;
                            }
                        } else {
                            // Raise invalid token error
                            raise_error(script, line_start_pos, "INVALID TOKEN", 
                                line_no, col_no, index,
                                "Found \"%s\", not a valid symbol\n", token_buffer
                            );
                            success = false;
                        }
                    }
                }
            }
        }
        if((curr_char == '\n' && prev_char != '\r') || curr_char == '\r') {
            is_comment = false;
            line_no++;
            is_new_line = true;
        } else if(curr_char == '\n' && prev_char == '\r') {
            // when EOL is CRLF
            col_no = 0;
        }
        if(reset_current_token) {
            token_col_no = current_token_type == T_OPERAND_STRING_VALUE 
                || current_token_type == T_OPERAND_STRING_TEMPLATE 
                ? string_pos.col_no : (col_no - strlen((const char*) token_buffer));
            if(is_new_line) {
                line_no--;
            } else {
                col_no--;
            }
            token_line_no = current_token_type == T_OPERAND_STRING_VALUE 
                || current_token_type == T_OPERAND_STRING_TEMPLATE 
                ? string_pos.line_no : line_no;
            current_token = add_token(current_token, (const char*) token_buffer, current_token_type, token_line_no, token_col_no, line_start_pos);
            (*number_of_tokens)++;
            token_buffer = (char*) realloc(token_buffer, sizeof(char) * 1);
            token_buffer[0] = '\0';
            index--;
            reset_current_token = false;
            prev_token_type = current_token_type;
            current_token_type = T_TOKEN_UNKNOWN;
            prev_char = curr_char == '\r' ? '\r' : '\0';
        } else {
            prev_char = curr_char;
        }
        if(is_new_line) {
            col_no = 0;
            is_new_line = false;
        }
    }
    if (success) {
        if(is_string) {
            raise_error(script, line_start_pos, "INVALID TOKEN", string_pos.line_no, string_pos.col_no, strlen(script)-1, 
                "Invalid string literal, missing string ending character %c", string_character);
            success = false;
        }
    }
    if(token_buffer) free(token_buffer);
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