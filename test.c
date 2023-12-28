/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

#include <stdio.h>
#include <string.h>
#include "cj5star.h"

const char *input = "{test:1337.456, otherwise:['a', 'b']}";

int main() {
    cj5_token tokens[32];
    cj5_result r = cj5_parse(input, strlen(input), tokens, 32, NULL);

    if(r.error != CJ5_ERROR_NONE) {
        if(r.error == CJ5_ERROR_OVERFLOW) {
            // you can use r.num_tokens to determine the actual token count and reparse
            printf("Error: line: %d, col: %d\n", r.error_line, r.error);    
        }
        return -1;
    }

    // lookup member of the root object
    unsigned int idx = 0;
    cj5_error_code err = cj5_find(&r, &idx, "test");
    if(err != CJ5_ERROR_NONE || tokens[idx].type != CJ5_TOKEN_NUMBER)
        return -1;

    // parse number-token as floating-point
    double num;
    err = cj5_get_number(&r, idx, &num);
    if(err == CJ5_ERROR_NONE) {
        printf("parsed %f\n", num);
    }

    return 0;
}
