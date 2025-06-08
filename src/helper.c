#ifdef MAKEFILECOMPILING
#include "helper.h"
#include "script.h"
#else
#include "../include/helper.h"
#include "../include/script.h"
#endif // MAKEFILECOMPILING
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

char* append_character(char* buffer, char ch) {
    size_t len = buffer ? strlen(buffer) : 0;
    if(buffer == NULL || (len > 0 && len % TOKEN_BUFFER_INC_SIZE == 0)) {
        buffer = (char*) realloc(buffer, sizeof(char) * (len + TOKEN_BUFFER_INC_SIZE + 1));
        if(!buffer) {
            return NULL;
        }
        memset(buffer + len, '\0', sizeof(char) * (TOKEN_BUFFER_INC_SIZE + 1));
    }
    buffer[len] = ch;
    return buffer;
}

int isodigit(int c) {
    return ((char) c >= '0') && ((char) c <= '7');
}

int isbdigit(int c) {
    return ((char) c == '0') || ((char) c == '1');
}

const char* unicode_seq_validator(char ch, int remaining_char_count, char* esc_seq_str) {
    if(isxdigit(ch)) {
        int pos = 4 - remaining_char_count;
        if(pos < 0) {
            return "Invalid position for universal character name";
        }
        esc_seq_str[pos] = ch;
    } else {
        return "Incomplete universal character name";
    }
    return NULL;
}


int get_esc_seq_char_count(char esc_seq_char) {
    if(esc_seq_char == '\\' || esc_seq_char == '"' 
        || esc_seq_char == '/' || esc_seq_char == 'b'
        || esc_seq_char == 'f' || esc_seq_char == 'n'
        || esc_seq_char == 'r' || esc_seq_char == 't') {
        return 0;
    } else if(esc_seq_char == 'u') {
        return 4;
    } else {
        return -1;
    }
}

// long get_token_index(const char* token_buffer, const long count, const char** list){
//     long int start_index = 0, end_index = count - 1;
//     while(start_index <= end_index) {
//         long int middle = start_index + (end_index - start_index)/2;
//         int comparison_result = strcmp(list[middle], token_buffer);
//         if(comparison_result == 0) {
//             return middle;
//         } else if(comparison_result < 0) {
//             start_index = middle + 1;
//         } else {
//             end_index = middle - 1;
//         }
//     }
//     return -1;
// }



// int raise_error(const char* script, size_t line_start_pos, const char* title, size_t line_no, size_t col_no, size_t pos_index, const char* fmt, ...) {
//     char* current_line = read_partial_script(script, line_start_pos);
//     if(!current_line) {
//         fprintf(stderr, "System error!\n\n");
//         return -1;
//     }
//     size_t col_no_temp = col_no == (size_t) -1 ? (strlen(current_line)+1) : col_no;
//     int wrc = fprintf(stderr, "[ERROR::%zu,%zu;%zu] %s\n\n", line_no, col_no_temp, pos_index, title);
//     int extra_space = fprintf(stderr, "  %zu| ", line_no);
//     wrc += extra_space;
//     wrc += fprintf(stderr, "%s\n", current_line);
//     char padfmt[32] = { 0 };
//     sprintf(padfmt, "%%%zus", col_no_temp+extra_space+1);
//     wrc += fprintf(stderr, padfmt, "^\n");
//     va_list args;
//     va_start(args, fmt);
//     wrc += vfprintf(stderr, fmt, args);
//     va_end(args);
//     free(current_line);
//     return wrc;
// }

int build_error_message(char* msg_plc_hldr, size_t pos_index, const char* fmt, ...) {
    if (!msg_plc_hldr || !fmt) return 0;

    int written = 0;

    // Write the prefix: "Syntax Error at [pos_index]. "
    written = snprintf(msg_plc_hldr, 1024, "Syntax Error at [%zu]. ", pos_index);

    // If snprintf fails
    if (written < 0) return 0;

    // Format the remaining message using va_list
    va_list args;
    va_start(args, fmt);
    int fmt_written = vsnprintf(msg_plc_hldr + written, 1024 - written, fmt, args);
    va_end(args);

    if (fmt_written < 0) return written;  // Return what we wrote before

    return written + fmt_written;
}