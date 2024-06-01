#ifdef MAKEFILECOMPILING
#include "script.h"
#else
#include "../include/script.h"
#endif // MAKEFILECOMPILING

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char* read_script_from_file(const char* filename, enum SCRIPT_RD_CODE* script_rd_code_ptr) {
    FILE* source_fp = fopen(filename, "r");
    if(source_fp == NULL) {
        *script_rd_code_ptr = SCRIPT_RD_OPENERR;
        return NULL;
    }
    if(fseek(source_fp, 0L, SEEK_END)) {
        *script_rd_code_ptr = SCRIPT_RD_FSIZEERR;
        fclose(source_fp);
        return NULL;
    }
    long seek_cur_offset = 0L;
    int ch;
    do {
        seek_cur_offset--;
        if(fseek(source_fp, seek_cur_offset, SEEK_END)) {
            *script_rd_code_ptr = SCRIPT_RD_FSIZEERR;
            fclose(source_fp);
            return NULL;
        }
        ch = fgetc(source_fp);
    } while(isspace(ch));
    if(fseek(source_fp, 0L, SEEK_END)) {
        *script_rd_code_ptr = SCRIPT_RD_FSIZEERR;
        fclose(source_fp);
        return NULL;
    }

    const long buffersize = ftell(source_fp) + seek_cur_offset + 3;
    if(fseek(source_fp, 0L, SEEK_SET)) {
        *script_rd_code_ptr = SCRIPT_RD_FPRESETERR;
        fclose(source_fp);
        return NULL;
    }
    if(feof(source_fp)) {
        *script_rd_code_ptr = SCRIPT_RD_FEOFERR;
        fclose(source_fp);
        return NULL;
    }
    char* source_code = (char*) calloc(buffersize, sizeof(char));
    if(source_code == NULL) {
        *script_rd_code_ptr = SCRIPT_RD_BUFALLOCERR;
        fclose(source_fp);
        return NULL;
    }
    memset(source_code, '\0', buffersize);
    fread(source_code, sizeof(char), buffersize-2, source_fp);
    strcat(source_code, "\n"); 
    if(fclose(source_fp) == EOF){
        *script_rd_code_ptr = SCRIPT_RD_FPCLOSEERR;
        free(source_code);
        return NULL;
    }
    return source_code;
}

char* read_partial_script(const char* script, size_t line_start_pos) {
    size_t index = line_start_pos;
    for(;!ISLINEENDINGCHAR(script[index]); index++);
    char* current_line = (char*) malloc(sizeof(char) * (index - line_start_pos + 1));
    memset(current_line, 0, (index - line_start_pos + 1));
    memmove(current_line, &(script[line_start_pos]), (index - line_start_pos));
    return current_line;
}