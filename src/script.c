#ifdef MAKEFILECOMPILING
#include "script.h"
#else
#include "../include/script.h"
#endif // MAKEFILECOMPILING

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
    const long buffersize = ftell(source_fp)+2;
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