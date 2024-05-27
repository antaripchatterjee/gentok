#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SCRIPT "func main(argv string[...]) {" \
    "\tstr string = 'Hello World\\41'" \
    "\tPrintf str" \
    "}"

#define ISLINEENDINGCHAR(CH) ((CH == '\0') || (CH == '\r') || (CH == '\n'))

enum SCRIPT_RD_CODE {
    SCRIPT_RD_NOERR = 0,
    SCRIPT_RD_OPENERR,
    SCRIPT_RD_FSIZEERR,
    SCRIPT_RD_FPRESETERR,
    SCRIPT_RD_FEOFERR,
    SCRIPT_RD_BUFALLOCERR,
    SCRIPT_RD_FPCLOSEERR,
    SCRIPT_RD_GTOKENERR
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
char* read_script_from_file(const char* filename, enum SCRIPT_RD_CODE* script_rd_code_ptr);
char* read_partial_script(const char* script, size_t line_start_pos);
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __SCRIPT_H__