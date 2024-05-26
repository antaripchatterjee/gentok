#ifdef MAKEFILECOMPILING
#include "helper.h"
#include "script.h"
#else
#include "../include/helper.h"
#include "../include/script.h"
#endif // MAKEFILECOMPILING
#include <ctype.h>

char* append_character(char* buffer, char ch) {
    size_t next_size = strlen(buffer) + 2;
    buffer = (char*) realloc(buffer, sizeof(char) * next_size);
    buffer[next_size-2] = ch;
    buffer[next_size-1] = '\0';
    return buffer;
}

int isodigit(int c) {
    return ((char) c >= '0') && ((char) c <= '7');
}

int isbdigit(int c) {
    return ((char) c == '0') || ((char) c == '1');
}

int octal_seq_validator(char ch, int* esc_seq_char_count, char* esc_seq_str, char* esc_seq_err) {
    if(isodigit(ch)) {
        int pos = 2 - (--(*esc_seq_char_count));
        esc_seq_str[pos] = ch;
        unsigned esc_seq_val = (unsigned) strtol(esc_seq_str, NULL, 8);
        if(esc_seq_val <= 0377) {
            return 1;
        } else {
            sprintf(esc_seq_err, "Invalid string literal, octal escape sequence '\\%s' is out of range", esc_seq_str);
            return 0;
        }
    } else {
        *esc_seq_char_count = 0;
    }
    return 1;
}

int hex_seq_validator(char ch, int* esc_seq_char_count, char* esc_seq_str, char* esc_seq_err) {
    if(isxdigit(ch)) {
        int pos = 2 - ((*esc_seq_char_count)--);
        esc_seq_str[pos] = ch;
    } else if(*esc_seq_char_count == 2) {
        sprintf(esc_seq_err, "Invalid string literal, '\\x' used with no following hex digits");
        return 0;
    } else {
        *esc_seq_char_count = 0;
    }
    return 1;
}

int universal_code_seq_validator(char ch, int* esc_seq_char_count, char* esc_seq_str, char* esc_seq_err) {
    static int max_esc_seq_char_count;
    if(max_esc_seq_char_count == 0) {
        max_esc_seq_char_count = *esc_seq_char_count;
    }
    char universal_code_flag = max_esc_seq_char_count == 4 ? 'u' : 'U';
    if(isxdigit(ch)) {
        int pos = max_esc_seq_char_count - ((*esc_seq_char_count)--);
        esc_seq_str[pos] = ch;
    } else {
        sprintf(esc_seq_err, "Invalid string literal, '\\%c' should be used "
            "with %d following hex digits but found '%c' (ASCII %d)",
            universal_code_flag, max_esc_seq_char_count, ch, ch);
        max_esc_seq_char_count = 0;
        return 0;
    }
    if(*esc_seq_char_count == 0) {
        max_esc_seq_char_count = 0;
        unsigned long esc_seq_val = strtoul(esc_seq_str, NULL, 16);
        if(esc_seq_val > 0x0010FFFF) {
            sprintf(esc_seq_err, "Invalid string literal, universal code '\\%c%s' is outside the UCS codespace",
                universal_code_flag, esc_seq_str);
            return 0;
        }
    }
    return 1;
}


int get_esc_seq_validation_rule(char esc_seq_char, esc_seq_validator_t* escape_seq_validator) {
    if(esc_seq_char == '\\' || esc_seq_char == '$' 
        || esc_seq_char == '\'' || esc_seq_char == '"' 
        || esc_seq_char == '`'  || esc_seq_char == '?' 
        || esc_seq_char == 'a' || esc_seq_char == 'b'
        || esc_seq_char == 'f' || esc_seq_char == 'n'
        || esc_seq_char == 'r' || esc_seq_char == 't'
        || esc_seq_char == 'v' || esc_seq_char == 't'
        || esc_seq_char == 's') {
        *escape_seq_validator = NULL;
        return 0;
    } else if(isodigit(esc_seq_char)) {
        *escape_seq_validator = &octal_seq_validator;
        return 2;
    } else if(esc_seq_char == 'x') {
        *escape_seq_validator = &hex_seq_validator;
        return 2;
    } else if(esc_seq_char == 'u') {
        *escape_seq_validator = &universal_code_seq_validator;
        return 4;
    } else if(esc_seq_char == 'U') {
        *escape_seq_validator = &universal_code_seq_validator;
        return 8;
    } else {
        *escape_seq_validator = NULL;
        return -1;
    }
}

long get_index_from(const char* token_buffer, const long count, const char** list){
    long int start_index = 0, end_index = count - 1;
    while(start_index <= end_index) {
        long int middle = start_index + (end_index - start_index)/2;
        int comparison_result = strcmp(list[middle], token_buffer);
        if(comparison_result == 0) {
            return middle;
        } else if(comparison_result < 0) {
            start_index = middle + 1;
        } else {
            end_index = middle - 1;
        }
    }
    return -1;
}


int raise_error(const char* script, size_t line_start_pos, const char* title, size_t line_no, size_t col_no, size_t pos_index, const char* fmt, ...) {
    int wrc = fprintf(stderr, "[ERROR::%zu,%zu;%zu] %s\n\n", line_no, col_no, pos_index, title);
    char* current_line = read_line_from_script(script, line_start_pos);
    int extra_space = fprintf(stderr, "  %zu| ", line_no);
    wrc += extra_space;
    wrc += fprintf(stderr, "%s\n", current_line);
    char padfmt[32] = { 0 };
    sprintf(padfmt, "%%%zus", col_no+extra_space+1);
    wrc += fprintf(stderr, padfmt, "^\n");
    va_list args;
    va_start(args, fmt);
    wrc += vfprintf(stderr, fmt, args);
    va_end(args);
    free(current_line);
    return wrc;
}