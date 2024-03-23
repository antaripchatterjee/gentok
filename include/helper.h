#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define RESERVED_KEYWORDS { \
    "as", "auto", "break", "class", \
    "continue", "def", "elif", "else", \
    "ensure", "false", "final", "for", "foreach", \
    "if", "import", "match", "null", "raise", "ref", \
    "rescue", "return", "true", "try", "when", "while" \
}

#define VALID_OPERATORS { \
    "!", "!=", "%%", \
    "%%=", "&", "&&", \
    "&=", "*", "**", \
    "**=", "*=", "+", \
    "+=", "-", "-=", \
    "...", "/", "/=", \
    "<", "<<", "<<=", \
    "<=", "=", "==", \
    ">", ">=", ">>", \
    ">>=", "^", "^=", \
    "|", "|=", "||", "~" \
}

#define VALID_SYMBOLS { \
    "(", ")", ",", ".", \
    ":", ";", "?", "@", \
    "[", "\\", "]", "{", "}" \
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int isodigit(int c);
long get_index_from(const char* token_buffer, const long count, const char** list);
int raise_invalid_token_error(unsigned long line_no, unsigned long column_no, unsigned long pos_index, const char* fmt, ...);
int raise_invalid_character_error(unsigned long line_no, unsigned long column_no, unsigned long pos_index, const char* fmt, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HELPER_H__