#ifndef __HELPER_H__
#define __HELPER_H__

#include <stddef.h>

#ifndef TOKEN_BUFFER_INC_SIZE
#define TOKEN_BUFFER_INC_SIZE 64
#endif // TOKEN_BUFFER__INC_SIZE

#define MIN(_A, _B) (_A > _B ? _B : _A)
#define MAX(_A, _B) (_A < _B ? _B : _A)
#define UCHAR(I) ((unsigned char) I)
#define IS_LITTLE_ENDIAN (((unsigned char*)((unsigned int[]){1}))[0])
#define ESCAPE_SEQ_MAX_LEN 5 // 4 hex digits + 1 for the escape character
#ifndef ERROR_MSG_SIZE
#define ERROR_MSG_SIZE 512
#endif // ERROR_MSG_SIZE
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
    : CH == '\t' ? "\\t" : (const char*)((char[]){ CH, '\0' }))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int isodigit(int c);
int isbdigit(int c);
// long get_token_index(const char* token_buffer, const long count, const char** list);
char* append_character(char* buffer, char ch);
const char* unicode_seq_validator(char ch, int remaining_char_count, char* esc_seq_str);
int get_esc_seq_char_count(char esc_seq_char);
// int raise_error(const char* script, size_t line_start_pos, const char* title, size_t line_no, size_t column_no, size_t pos_index, const char* msgfmt, ...);
int build_error_message(char* msg_plc_hldr, size_t pos_index, const char* fmt, ...);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HELPER_H__