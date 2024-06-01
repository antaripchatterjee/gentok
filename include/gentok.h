#ifndef __GENTOK_H__
#define __GENTOK_H__

#ifdef MAKEFILECOMPILING
#include "script.h"
#include "helper.h"
#include "token.h"
#else
#include "../include/script.h"
#include "../include/helper.h"
#include "../include/token.h"
#endif // MAKEFILECOMPILING

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool tokenize(const char* script, struct token_t* token, long* number_of_tokens);
long free_tokens(struct token_t* token);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __GENTOK_H__