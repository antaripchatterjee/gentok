#ifdef MAKEFILECOMPILING
#include "helper.h"
#else
#include "../include/helper.h"
#endif // MAKEFILECOMPILING

long get_index_from(const char* token_buffer, const long count, const char** list){
    // const long int count = sizeof(list)/sizeof(char*);
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

int raise_invalid_token_error(unsigned long line_no, unsigned long col_no, unsigned long pos_index, const char* fmt, ...){
    int wrc = fprintf(stdout, "[ERR] Invalid token found at position %ld::(%ld,%ld)\n\t", pos_index, line_no, col_no);
    va_list args;
    va_start(args, fmt);
    wrc += vfprintf(stderr, fmt, args);
    va_end(args);
    return wrc;
}


int raise_invalid_character_error(unsigned long line_no, unsigned long col_no, unsigned long pos_index, const char* fmt, ...){
    int wrc = fprintf(stdout, "[ERR] Invalid character found at position %ld::(%ld,%ld)\n\t", pos_index, line_no, col_no);
    va_list args;
    va_start(args, fmt);
    wrc += vfprintf(stderr, fmt, args);
    va_end(args);
    return wrc;
}