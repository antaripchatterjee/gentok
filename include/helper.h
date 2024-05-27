#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define RESERVED_KEYWORDS { \
    "bool", "break", "char", "continue", \
    "else", "enum", "f128", "f32", "f64", \
    "false", "for", "func", "i16", "i32", \
    "i64", "i8", "if", "match", "mixin", \
    "new", "null", "raise", "repeat", \
    "rescue", "return", "string", "struct", \
    "template", "true", "u16", "u32", \
    "u64", "u8", "union", "until", "when", "while" \
}

#define VALID_SYMBOLS { \
    "!", "!=", "%", "%=", "&", \
    "&&", "&=", "(", ")", "*", \
    "**", "**=", "*=", "+", \
    "+=", ",", "-", "-=", "->", \
    ".", "..", "...", "/", "/=", \
    ":", "::", ":=", "<", "<<", \
    "<<=", "<=", "=", "==", ">", \
    ">=", ">>", ">>=", "?", "??", \
    "@", "[", "]", "^", "^=", "{", \
    "|", "|=", "||", "}", "~" \
}

#define MIN(_A, _B) (_A > _B ? _B : _A)
#define UCHAR(I) ((unsigned char) I)
#define IS_LITTLE_ENDIAN (((unsigned char*)((unsigned int[]){1}))[0])
#define ESCAPE_SEQ_MAX_LEN 9
#define ESCAPE_SEQ_ERR_MSG_SZ 128
#define REPRCHAR(CH) (CH == '\\' ? "\\\\" \
    : CH == '\'' ? "\\\'" \
    : CH == '\"' ? "\\\"" \
    : CH == '`'  ? "\\`"  \
    : CH == '\a' ? "\\a"  \
    : CH == '\b' ? "\\b"  \
    : CH == '\f' ? "\\f"  \
    : CH == '\n' ? "\\n"  \
    : CH == '\r' ? "\\r"  \
    : CH == '\v' ? "\\v"  \
    : CH == '\t' ? "\\t" : (const char*)((char[]){ CH }))

typedef int(*esc_seq_validator_t)(char, int*, char*, char*);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int isodigit(int c);
int isbdigit(int c);
long get_index_from(const char* token_buffer, const long count, const char** list);
char* append_character(char* buffer, char ch);
int octal_seq_validator(char ch, int* esc_seq_char_count, char* esc_seq_str, char* esc_seq_err);
int hex_seq_validator(char ch, int* esc_seq_char_count, char* esc_seq_str, char* esc_seq_err);
int universal_code_seq_validator(char ch, int* esc_seq_char_count, char* esc_seq_str, char* esc_seq_err);
int get_esc_seq_validation_rule(char esc_seq_char, esc_seq_validator_t* escape_seq_validator);
int raise_error(const char* script, size_t line_start_pos, const char* title, size_t line_no, size_t column_no, size_t pos_index, const char* msgfmt, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HELPER_H__