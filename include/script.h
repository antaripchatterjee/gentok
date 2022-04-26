#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SCRIPT "import sys.{argv, print} as [argv, print]\n" \
    "\ndef main(...) {\n" \
    "\t# This is a comment\n" \
    "\tfinal auto y = 0x78a3\n" \
    "\tif argv.count < -2+4 {\n" \
    "\t\tprint('Hello, World!')\n" \
    "\t} else { \n" \
    "\t\tforeach argv as [index, arg] {\n" \
    "\t\t\tprint('ARGV[${index}] -> ${arg}')\n" \
    "\t\t}\n" \
    "\t}\n" \
    "}\n"

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
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __SCRIPT_H__