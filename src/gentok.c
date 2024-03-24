#ifdef MAKEFILECOMPILING
#include "gentok.h"
#else
#include "../include/gentok.h"
#endif // MAKEFILECOMPILING

#include <stdio.h>

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

static bool treat_symbol_as_operator(enum TOKENTYPE_E prev_token_type) {
    return prev_token_type == T_OPERAND_IDENTIFIER ||
        prev_token_type == T_OPERAND_STRING_VALUE ||
        prev_token_type == T_OPERAND_BINARY_NUMBER ||
        prev_token_type == T_OPERAND_OCTAL_NUMBER ||
        prev_token_type == T_OPERAND_DECIMAL_NUMBER ||
        prev_token_type == T_OPERAND_HEXDECIMAL_NUMBER ||
        prev_token_type == T_SYMBOL_CLOSING_CURLY_BRACKET ||
        prev_token_type == T_SYMBOL_CLOSING_PARANANTHESIS ||
        prev_token_type == T_SYMBOL_CLOSING_SQUARE_BRACKET;
}

static char get_char_value(char read_char, bool is_template_string) {
    if(read_char == '\\' ) return '\\';
    else if(read_char == '$' && is_template_string) return '$';
    else if(read_char == '\'') return '\'';
    else if(read_char == '"' ) return '"';
    else if(read_char == '`' ) return '`';
    else if(read_char == '?' ) return '?';
    else if(read_char == '`' ) return '`';
    else if(read_char == 'a') return '\a';
    else if(read_char == 'b') return '\b';
    else if(read_char == 'f') return '\f';
    else if(read_char == 'n') return '\n';
    else if(read_char == 'r') return '\r';
    else if(read_char == 't') return '\t';
    else if(read_char == 'v') return '\v';
    else if(read_char == 't') return '\t';
    else if(read_char == 't') return '\t';
    else if(isodigit(read_char)) return ESCAPE_CHARACTER;
    else if(read_char == 'x') return ESCAPE_CHARACTER;
    else if(read_char == 'u' || read_char == 'U') return ESCAPE_CHARACTER;
    else return CANCEL_CHARACTER;
} 

bool tokenize(const char* script, struct token_t* token, long* number_of_tokens) {
    const char* reserved_keywords[] = RESERVED_KEYWORDS;
    const char* valid_operators[] = VALID_OPERATORS;
    const char* valid_symbols[] = VALID_SYMBOLS;

    const long count_reserved_keywords = sizeof(reserved_keywords)/sizeof(char*);
    const long count_valid_operators = sizeof(valid_operators)/sizeof(char*);
    const long count_valid_symbols = sizeof(valid_symbols)/sizeof(char*);

    char curr_char, prev_char = '\0';
    char string_character = '\0';

    bool success = true;

    bool is_word = false, is_number = false, is_comment = false,
        is_string = false, is_symbol = false, is_operator = false,
        is_sign = false, is_template_string = false;
    short number_base_system = 0;
    bool exp_appended = false, decimal_appended = false, allow_char_as_num = false;
    bool reset_current_token = false;
    
    enum TOKENTYPE_E current_token_type = T_TOKEN_UNKNOWN, prev_token_type;
    struct token_t* current_token = token;
    char* token_buffer = (char*) malloc(sizeof(char) * 1);
    token_buffer[0] = '\0';

    size_t line_no = 1, col_no = 0;

    for(size_t index = 0; (curr_char = script[index]) != '\0' && success; index++) {
        col_no++;
        if((curr_char == '\n' && prev_char != '\r') || curr_char == '\r') {
            line_no++;
            col_no = 1;
        }
        if(!is_comment) {
            if(!isspace(curr_char) && !isprint(curr_char)) {
                raise_invalid_character_error(index, line_no, col_no,
                    "Found '%c'(ASCII %d)\n", curr_char, (int) curr_char);
                success = false;
            }
            if(is_string) {
                if(prev_char == ESCAPE_CHARACTER) {
                    char char_value = get_char_value(curr_char, is_template_string);
                    if(char_value == CANCEL_CHARACTER) {
                        raise_invalid_character_error(index, line_no, col_no,
                            "Invalid escape sequence '%c'(ASCII %d)\n", curr_char, (int) curr_char);
                        success = false;
                    } else if(char_value == ESCAPE_CHARACTER) {
                        char str_seq_text[17] = { 0 };
                        unsigned str_seq_start_index = 0;
                        int str_seq_max_len = -1;
                        int str_seq_base = -1;
                        digit_validator_t digit_validator = NULL;
                        if(isodigit(curr_char)) {
                            str_seq_max_len = 3;
                            str_seq_base = 8;
                            digit_validator = &isodigit;
                            str_seq_text[0] = curr_char;
                            str_seq_start_index = 1;
                        } else if(curr_char == 'u') {
                            str_seq_max_len = 4;
                            str_seq_base = 16;
                            digit_validator = &isxdigit;
                        } else if(curr_char == 'U') {
                            str_seq_max_len = 8;
                            str_seq_base = 16;
                            digit_validator = &isxdigit;
                        } else if(curr_char == 'x') {
                            str_seq_max_len = 2;
                            str_seq_base = 16;
                            digit_validator = &isxdigit;
                        }

                        while(str_seq_start_index < str_seq_max_len) {
                            char ch = script[index+1];
                            if(!digit_validator(ch)) break;
                            index++;
                            str_seq_text[str_seq_start_index] = ch;
                            str_seq_start_index++;
                            col_no++;
                        }
                        
                        if(str_seq_start_index) {
                            if((curr_char == 'u' || curr_char == 'U') && str_seq_start_index != str_seq_max_len) {
                                raise_invalid_character_error(index, line_no, col_no,
                                    "Invalid unicode sequence\n");
                                success = false;   
                            } else {
                                size_t additional_space_sz = 0;
                                uint32_t seq_as_uint = (uint32_t) strtoul(str_seq_text, (char**) NULL, str_seq_base);
                                unsigned char seq_as_bytes[sizeof(uint32_t)];
                                memmove(seq_as_bytes, &seq_as_uint, sizeof(uint32_t));
                                if(!IS_LITTLE_ENDIAN) {
                                    unsigned char _swap_temp = seq_as_bytes[0];
                                    seq_as_bytes[0] = seq_as_bytes[3];
                                    seq_as_bytes[3] = _swap_temp;
                                    _swap_temp = seq_as_bytes[1];
                                    seq_as_bytes[1] = seq_as_bytes[2];
                                    seq_as_bytes[2] = _swap_temp; 
                                }
                                unsigned char utfBytes[5] = { 
                                    UCHAR(0),
                                    UCHAR(128), 
                                    UCHAR(128),
                                    UCHAR(128),
                                    UCHAR(0)
                                };
                                if(seq_as_uint >= 0x0 && seq_as_uint <= 0x7f) {
                                    // 1 Byte
                                    additional_space_sz = 1;
                                    utfBytes[0] = UCHAR(0) | seq_as_bytes[0];
                                    utfBytes[1] = utfBytes[2] = utfBytes[3] = UCHAR(0);
                                } else if(seq_as_uint <= 0x07ff) {
                                    // 2 Bytes
                                    additional_space_sz = 2;
                                    utfBytes[0] = UCHAR(192) | ((seq_as_bytes[1] & UCHAR(7)) << 2) | ((seq_as_bytes[0] & UCHAR(192)) >> 6);
                                    utfBytes[1] = utfBytes[1] | (seq_as_bytes[0] & UCHAR(63));
                                    utfBytes[2] = utfBytes[3] = UCHAR(0);
                                } else if(seq_as_uint <= 0xFFFF) {
                                    // 3 Bytes
                                    additional_space_sz = 3;
                                    utfBytes[0] = UCHAR(224) | ((seq_as_bytes[1] & UCHAR(240)) >> 4);
                                    utfBytes[1] = utfBytes[1] | ((seq_as_bytes[1] & UCHAR(15)) << 2) | ((seq_as_bytes[0] & UCHAR(192)) >> 6);
                                    utfBytes[2] = utfBytes[2] | (seq_as_bytes[0] & UCHAR(63));
                                    utfBytes[3] = UCHAR(0);
                                } else if(seq_as_uint <= 0x10FFFF) {
                                    // 4 Bytes
                                    additional_space_sz = 4;
                                    utfBytes[0] = UCHAR(240) | ((seq_as_bytes[2] & UCHAR(31)) >> 2);
                                    utfBytes[1] = utfBytes[1] | ((seq_as_bytes[2] & UCHAR(3)) << 4) | ((seq_as_bytes[1] & UCHAR(240)) >> 4);
                                    utfBytes[2] = utfBytes[2] | ((seq_as_bytes[1] & UCHAR(15)) << 2) | ((seq_as_bytes[0] & UCHAR(192)) >> 6);
                                    utfBytes[3] = utfBytes[3] | (seq_as_bytes[0] & UCHAR(63));
                                    printf("%x %x %x %x\n", utfBytes[0], utfBytes[1], utfBytes[2], utfBytes[3]);
                                } else {
                                    // Error
                                    success = false;
                                }
                                
                                if(success && additional_space_sz) {
                                    size_t token_buffer_len = strlen(token_buffer);
                                    token_buffer = (char*) realloc(token_buffer, 
                                        sizeof(char) * (token_buffer_len + additional_space_sz + 1));
                                    if(token_buffer) {
                                        memset(&token_buffer[token_buffer_len], 0, additional_space_sz + 1);
                                        memmove(&token_buffer[token_buffer_len], utfBytes, additional_space_sz);
                                    } else {
                                        // error
                                        success = false;
                                    }
                                }
                            }
                        } else {
                            raise_invalid_character_error(index, line_no, col_no,
                                "Invalid unicode sequence\n");
                            success = false;  
                        }
                        curr_char = '\0';
                    } else {
                        token_buffer = append_character(token_buffer, char_value);
                    }
                } else if(curr_char == '\\') {
                    curr_char = ESCAPE_CHARACTER;
                } else {
                    if(curr_char == string_character) {
                        current_token_type = T_OPERAND_STRING_VALUE;
                        reset_current_token = true;
                        index++;
                        is_string = false;
                    } else {
                        token_buffer = append_character(token_buffer, curr_char);
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
                        if(!isodigit(curr_char)) {
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
                    } else if(isalnum(curr_char) || curr_char == '_') {
                        // add the token as unary operator and reset the token buffer
                        current_token_type = token_buffer[strlen(token_buffer) - 1] == '+'
                            ? T_OPERATOR_UNARY_SIGN_PLUS : T_OPERATOR_UNARY_SIGN_MINUS;
                        is_sign = false;
                        reset_current_token = true;
                    } else if(curr_char == '+' || curr_char == '-') {
                        // update the unary operator
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
                } else if(is_operator) {
                    token_buffer = append_character(token_buffer, curr_char);
                    long token_index = get_index_from(token_buffer, count_valid_operators, valid_operators);
                    if(token_index == -1) {
                        token_buffer[strlen(token_buffer)-1] = '\0';
                        reset_current_token = true;
                        is_operator = false;
                    } else {
                        current_token_type = (enum TOKENTYPE_E) (token_index + OPERATOR_TOKEN_OFFSET);
                    }
                } else if(is_symbol) {
                    if(curr_char == '.' && prev_char == '.')  {
                        token_buffer = append_character(token_buffer, curr_char);
                        current_token_type = T_TOKEN_UNKNOWN;
                        is_symbol = false;
                    } else {
                        token_buffer = append_character(token_buffer, curr_char);
                        long token_index = get_index_from(token_buffer, count_valid_symbols, valid_symbols);
                        if(token_index == -1) {
                            token_buffer[strlen(token_buffer)-1] = '\0';
                            reset_current_token = true;
                            is_symbol = false;
                        } else {
                            current_token_type = (enum TOKENTYPE_E) (token_index + SYMBOL_TOKEN_OFFSET);
                        }
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
                        // token_buffer = append_character(token_buffer, curr_char);
                        string_character = curr_char;
                        is_string = true;
                        if(curr_char == '`') is_template_string = true;
                    } else if(!isspace(curr_char)) {
                        token_buffer = append_character(token_buffer, curr_char);
                        long token_index = get_index_from(token_buffer, count_valid_operators, valid_operators);
                        if(token_index != -1) {
                            if((curr_char == '+' || curr_char == '-') && !treat_symbol_as_operator(prev_token_type)) {
                                is_sign = true;
                            } else {
                                current_token_type = (enum TOKENTYPE_E) (token_index + OPERATOR_TOKEN_OFFSET);
                                is_operator = true;
                            }
                        } else {
                            token_index = get_index_from(token_buffer, count_valid_symbols, valid_symbols);
                            if(token_index != -1) {
                                current_token_type = (enum TOKENTYPE_E) (token_index + SYMBOL_TOKEN_OFFSET);
                                is_symbol = true;
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