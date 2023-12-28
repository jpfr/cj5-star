// MIT License
//
// Copyright (c) 2010 Serge Zaitsev
// Copyright (c) 2020 Sepehr Taghdisian
// Copyright (c) 2022 Julius Pfrommer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __CJ5_H_
#define __CJ5_H_

#ifdef __cplusplus
# define CJ5_API extern "C"
#else
# define CJ5_API
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum cj5_token_type {
    CJ5_TOKEN_OBJECT = 0,
    CJ5_TOKEN_ARRAY,
    CJ5_TOKEN_NUMBER,
    CJ5_TOKEN_STRING,
    CJ5_TOKEN_BOOL,
    CJ5_TOKEN_NULL
} cj5_token_type;

typedef enum cj5_error_code {
    CJ5_ERROR_NONE = 0,
    CJ5_ERROR_INVALID,       // Invalid character/syntax
    CJ5_ERROR_INCOMPLETE,    // Incomplete JSON string
    CJ5_ERROR_OVERFLOW,      // Token buffer overflow (see cj5_result.num_tokens)
    CJ5_ERROR_NOTFOUND
} cj5_error_code;

typedef struct cj5_token {
    cj5_token_type type;
    unsigned int start;     // Start position in the json5 string
    unsigned int end;       // Position of the last character (included)
    unsigned int size;      // For objects and arrays the number of direct
                            // children. Note that this is *not* the number of
                            // overall (recursively nested) child tokens. For
                            // other tokens the length of token in the json
                            // encoding.
    unsigned int parent_id; // The root object is at position zero. It is an
                            // object that has itself as parent.
} cj5_token;

typedef struct cj5_result {
    cj5_error_code error;
    unsigned int error_line;
    unsigned int error_col;
    unsigned int num_tokens;
    const cj5_token* tokens;
    const char* json5;
} cj5_result;

typedef struct cj5_options {
    bool stop_early; /* Return when the first element was parsed. Otherwise an
                      * error is returned if the input was not fully
                      * processed. (default: false) */
} cj5_options;

/* Options can be NULL */
CJ5_API cj5_result
cj5_parse(const char *json5, unsigned int len,
          cj5_token *tokens, unsigned int max_tokens,
          cj5_options *options);

CJ5_API cj5_error_code
cj5_get_bool(const cj5_result *r, unsigned int tok_index, bool *out);

CJ5_API cj5_error_code
cj5_get_number(const cj5_result *r, unsigned int tok_index, double *out);

// Replaces escape characters, utf8 codepoints, etc.
// The buffer shall have a length of at least token->size + 1.
// Upon success, the length is written to buflen.
// The output string is terminated with \0.
CJ5_API cj5_error_code
cj5_get_str(const cj5_result *r, unsigned int tok_index,
            char *buf, unsigned int *buflen);

// Skips the (nested) structure that starts at the current index. The index is
// updated accordingly. Afterwards it points to the beginning of the following
// structure.
//
// Attention! The index can point to the first element after the token array if
// the root object is skipped.
//
// Cannot fail as long as the token array is the result of cj5_parse.
CJ5_API void
cj5_skip(const cj5_result *r, unsigned int *tok_index);

// Lookup of a key within an object (linear search).
// The current token (index) must point to an object.
// The error code CJ5_ERROR_NOTFOUND is returned if the key is not present.
// Otherwise the index is updated to point to the value associated with the key.
CJ5_API cj5_error_code
cj5_find(const cj5_result *r, unsigned int *tok_index, const char *key);

#endif /* __CJ5_H_ */
