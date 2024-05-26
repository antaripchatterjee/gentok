#ifdef MAKEFILECOMPILING
#include "gentok.h"
#else
#include "./include/gentok.h"
#endif // MAKEFILECOMPILING

int main(int argc, const char* argv[]) {
    enum SCRIPT_RD_CODE script_rd_code = SCRIPT_RD_NOERR;
    const char* script = argc > 1 ? (const char*)
        read_script_from_file(argv[1], &script_rd_code) : SCRIPT;
    if(!script) {
        printf("FATAL: INPUT FILE ERROR [CODE: %d]\n", (int) script_rd_code);
        return -1;
    }
    struct token_t token = {
        .token_type = T_TOKEN_UNKNOWN,
        .token_buffer = NULL,
        .pos = { 0UL, 0UL },
        .next_token = NULL
    };

    long number_of_tokens = 0;

    bool success = tokenize(script, &token, &number_of_tokens);
    if(success) {
        // process the tokens
        printf("number of tokens : %ld\n", number_of_tokens);
        struct token_t* t = &token;
        for(long i = 0; i < number_of_tokens; i++){
            printf("token[%ld at %zu:%zu] -> %s : %d\n", i+1, t->pos.line_no, t->pos.col_no, t->token_buffer, (int) t->token_type);
            t = t->next_token;
        }
    }
    if(free_tokens(&token) == number_of_tokens)
        puts("\n\nTokens freed");
    else
        puts("\n\nFailed to free the tokens");
    return 0;
}