#ifdef MAKEFILECOMPILING
#include "gentok.h"
#else
#include "../include/gentok.h"
#endif // MAKEFILECOMPILING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


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


bool tokenize(const char* script, struct token_t* token, long* number_of_tokens) {
    const char* reserved_keywords[] = RESERVED_KEYWORDS;
    const char valid_symbols[] = VALID_SYMBOLS;

    const long count_reserved_keywords = sizeof(reserved_keywords)/sizeof(char*);
    const long count_valid_symbols = sizeof(valid_symbols)/sizeof(char);

    char curr_char, prev_char = '\0';

    bool success = true;

    bool is_number = false, is_number_beginning = false, is_resv_const = false,
        is_string = false, is_new_line = false;
    bool exp_appended = false, decimal_appended = false, allow_char_as_num = false;
    bool push_current_token = false;
    
    enum TOKENTYPE_E current_token_type = T_TOKEN_UNKNOWN; //, prev_token_type;
    struct token_t* current_token = token;
    char* token_buffer = (char*) malloc(sizeof(char) * TOKEN_BUFFER_INC_SIZE);
    token_buffer[0] = '\0';

    size_t line_no = 1, col_no = 1, line_start_pos = 0;
    size_t token_line_no, token_col_no;
    struct token_pos_t long_token_pos = { 0UL, 0UL };
    char esc_seq_str[ESCAPE_SEQ_MAX_LEN] = { 0 };
    char static_error_msg[ERROR_MSG_SIZE] = { 0 };
    esc_seq_validator_t escape_seq_validator = NULL;
    int escape_seq_char_count = -1;
    for(size_t index = 0; (curr_char = script[index]) != '\0' && success; col_no++, index++) {
        printf("Processing character '%c' (ASCII %d) at index %zu, line %zu, col %zu\n", curr_char, curr_char, index, line_no, col_no);
        if((prev_char == '\r' || prev_char == '\n') && !is_string) {
            line_start_pos = index;
        }
        if(/* !is_comment */ true) {
            if(!isspace(curr_char) && !isprint(curr_char)) {
                raise_error(script, line_start_pos, "SYNTAX ERROR", line_no, col_no, index,
                    "Found invalid character %s (ASCII %d)\n", REPRCHAR(curr_char), curr_char);
                success = false;
            }
            if(is_string) {
                if((curr_char == '\n' || curr_char == '\r')) {
                    raise_error(script, line_start_pos, "SYNTAX ERROR", long_token_pos.line_no, long_token_pos.col_no, index, 
                        "Invalid string literal, missing string ending character \"");
                    success = false;
                }
                if(success) {
                    token_buffer = append_character(token_buffer, curr_char);
                    if(prev_char == ESCAPE_CHARACTER) {
                        memset(static_error_msg, 0, ERROR_MSG_SIZE);
                        if(escape_seq_validator && escape_seq_char_count > 0) {
                            if(!escape_seq_validator(curr_char, &escape_seq_char_count, esc_seq_str, static_error_msg)) {
                                // error
                                raise_error(script, line_start_pos, "SYNTAX ERROR", long_token_pos.line_no, long_token_pos.col_no, index, static_error_msg);
                                memset(static_error_msg, 0, ERROR_MSG_SIZE);
                                success = false;
                            }
                        } else if(!escape_seq_validator) {
                            escape_seq_char_count = get_esc_seq_validation_rule(curr_char, &escape_seq_validator);
                            if(escape_seq_char_count < 0) {
                                raise_error(script, line_start_pos, "SYNTAX ERROR", long_token_pos.line_no, long_token_pos.col_no, index,
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
                    } else if(curr_char == '"' && prev_char != ESCAPE_CHARACTER) {
                        current_token_type = T_OPERAND_STRING_VALUE;
                        push_current_token = true;
                        index++;
                        col_no++;
                        is_string = false;
                    }
                    if(escape_seq_char_count > 0) {
                        curr_char = ESCAPE_CHARACTER;
                    }
                }
            } else {
                if(is_resv_const) {
                    if(isalnum(curr_char) || curr_char == '_') {
                        token_buffer = append_character(token_buffer, curr_char);
                    } else {
                        // long token_index = get_token_index(token_buffer, count_reserved_keywords, reserved_keywords);
                        int token_index = -1;
                        for(int i = 0; i < count_reserved_keywords; i++) {
                            if(strcmp(reserved_keywords[i], token_buffer) == 0) {
                                token_index = i;
                                break;
                            }
                        }
                        if(token_index != -1) {
                            current_token_type = (enum TOKENTYPE_E) (token_index + RESERVED_KEYWORD_OFFSET);
                        } else {
                            success = false;
                            // TODO: raise_error as invalid reserved keyword
                        }
                        is_resv_const = false;
                        push_current_token = true;
                    }
                } else if(is_number) {
                    if(is_number_beginning) {
                        is_number_beginning = false;
                        if(prev_char == '0') {
                            if(curr_char == '.' || tolower(curr_char) == 'e') {
                                decimal_appended = true;
                                if(curr_char == 'e') {
                                    exp_appended = true;
                                }
                            } else if(curr_char != ',' && curr_char != '}' && !isspace(curr_char)) {
                                // TODO: raise_error as leading zero is not allowed
                                success = false;
                                allow_char_as_num = false;
                                raise_error(script, line_start_pos, "SYNTAX ERROR", line_no, col_no, index,
                                    "Unexpected occurence of '%s' (ASCII %d),"
                                    " not a valid decimal digit after leading zero.\n", REPRCHAR(curr_char), curr_char
                                );
                            } else {
                                allow_char_as_num = false;
                            }
                        } else {
                            if(isdigit(curr_char) || curr_char == '.' || tolower(curr_char) == 'e') {
                                if(curr_char == '.') {
                                    decimal_appended = true;
                                } else if(tolower(curr_char) == 'e') {
                                    decimal_appended = true;
                                    exp_appended = true;
                                }
                            } else if(curr_char != ',' && curr_char != '}' && !isspace(curr_char)) {
                                // Raise invalid syntax error
                                allow_char_as_num = false;
                                raise_error(script, line_start_pos, "SYNTAX ERROR", line_no, col_no, index,
                                    "Unexpected occurence of '%s' (ASCII %d),"
                                    " not a valid decimal digit.\n", REPRCHAR(curr_char), curr_char
                                );
                                success = false;
                            } else {
                                allow_char_as_num = false;
                            }
                        }
                    } else {
                        if(isdigit(curr_char) || (curr_char == '.' && !decimal_appended)
                            || (tolower(curr_char) == 'e' && !exp_appended)
                            || ((curr_char == '+' || curr_char == '-') && tolower(prev_char) == 'e' && exp_appended)) {
                            if(curr_char == '.') {
                                decimal_appended = true;
                            } else if(tolower(curr_char) == 'e') {
                                decimal_appended = true;
                                exp_appended = true;
                            }
                        } else if(curr_char != ',' && curr_char != '}' && !isspace(curr_char)) {
                            // Raise the invalid syntax error
                            allow_char_as_num = false;
                            raise_error(script, line_start_pos, "SYNTAX ERROR", line_no, col_no, index,
                                "Unexpected occurence of '%s' (ASCII %d),"
                                " not a valid decimal digit.\n", REPRCHAR(curr_char), curr_char
                            );
                            success = false;
                        } else {
                            allow_char_as_num = false;
                        }
                    }
                    if(allow_char_as_num) {
                        token_buffer = append_character(token_buffer, tolower(curr_char));
                    } else {
                        current_token_type = exp_appended || decimal_appended 
                            ? T_OPERAND_DECIMAL_DOUBLE : T_OPERAND_DECIMAL_INTEGER;
                        push_current_token = true;
                        is_number = false;
                        exp_appended = decimal_appended = allow_char_as_num = false;
                    }
                } else {
                    if(isalpha(curr_char) || curr_char == '_') {
                        token_buffer = append_character(token_buffer, tolower(curr_char));
                        is_resv_const = true;
                        long_token_pos.line_no = line_no;
                        long_token_pos.col_no = col_no;
                    } else if(isdigit(curr_char) || curr_char == '-') {
                        token_buffer = append_character(token_buffer, curr_char);
                        is_number = true;
                        is_number_beginning = true;
                        allow_char_as_num = true;
                    } else if(curr_char == '"') {
                        token_buffer = append_character(token_buffer, curr_char);
                        is_string = true;
                        long_token_pos.line_no = line_no;
                        long_token_pos.col_no = col_no;
                    } else if(!isspace(curr_char)) {
                        // long token_index = get_token_index(token_buffer, count_valid_symbols, valid_symbols);
                        int token_index = -1;
                        for(int i = 0; i < count_valid_symbols; i++) {
                            if(valid_symbols[i] == curr_char) {
                                token_index = i;
                                break;
                            }
                        }
                        if(token_index != -1) {
                            current_token_type = (enum TOKENTYPE_E) (token_index + SYMBOL_TOKEN_OFFSET);
                            token_buffer = append_character(token_buffer, curr_char);
                            push_current_token = true;
                            index++;
                            long_token_pos.line_no = line_no;
                            long_token_pos.col_no = col_no;
                        } else {
                            // Raise invalid token error
                            raise_error(script, line_start_pos, "SYNTAX ERROR", 
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
            line_no++;
            is_new_line = true;
        } else if(curr_char == '\n' && prev_char == '\r') {
            // when EOL is CRLF
            col_no = 0;
        }
        if(push_current_token) {
            token_col_no = current_token_type == T_OPERAND_STRING_VALUE
                ? long_token_pos.col_no : (col_no - strlen((const char*) token_buffer));
            if(is_new_line) {
                line_no--;
            } else {
                col_no--;
            }
            token_line_no = current_token_type == T_OPERAND_STRING_VALUE
                ? long_token_pos.line_no : line_no;
            current_token = add_token(current_token, (const char*) token_buffer, current_token_type, token_line_no, token_col_no, line_start_pos);
            (*number_of_tokens)++;
            token_buffer = (char*) realloc(token_buffer, sizeof(char) * 1);
            token_buffer[0] = '\0';
            index--;
            push_current_token = false;
            // prev_token_type = current_token_type;
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
            raise_error(script, line_start_pos, "SYNTAX ERROR", long_token_pos.line_no, long_token_pos.col_no, strlen(script)-1, 
                "Invalid string literal, missing string ending character \"");
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