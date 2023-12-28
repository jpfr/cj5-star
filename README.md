# cj5star

Improved JSON5 parser in C99. cj5star is derived from
[cj5](https://github.com/septag/cj5) and heavily rewritten/extended by @jpfr
(Julius Pfrommer). cj5star is under the MIT license.

## (Extended) JSON5
JSON5 is less strict than standard JSON and easier to read and write. For more
info see [JSON5](https://json5.org/). The following JSON5 features have been
implemented:

- Object keys may be an ECMAScript 5.1 IdentifierName.
- Objects may have a single trailing comma.
- Strings may be single quoted.
- Strings may span multiple lines by escaping new line characters.
- Strings may include character escapes.
- Numbers may be hexadecimal.
- Numbers may have a leading or trailing decimal point.
- Numbers may be IEEE754 positive infinity, negative infinity, and NaN.
- Numbers may begin with an explicit plus sign.
- Single and multi-line comments are allowed (//, /* */).
- Additional white space characters are allowed.

The following extensions in addition to JSON5 have been added to cj5star. The
goal is to make it even more usable as a replacement for .ini files for program
configuration or to add JSON-blobs as part of bigger configuration files.

- The root object may omit the surrounding brackets
- The root object can be an array or a primitive value
- A free-standing hash ('#') comments out until the end of the line.
- Strings may include unescaped utf8 bytes
- Optionally: Stop early when the first encountered JSON element (object, array,
              value) has been successfully parsed. Do not return an error when
              the input string was not processed to its full length. This allows
              the detection of JSON sub-strings as part of an input "lexer".

## Features
- No memory allocations, all allocations are managed on the user side
- Easy to use portable C API
- Fast and Minimal

## Usage
The main function to parse json is `cj5_parse`. The user pre-allocates memory
for the tokens and provides the maximum count. The result is returned in a
`cj5_result` struct, and `num_tokens` represents the actual token count that is
parsed. In the case of errors, `cj_result.error` will be set to an error code.
Here's a quick example of the usage.

```c
#include "cj5star.h"

const char *input = "{test:1337.456, otherwise:['a', 'b']}";

cj5_token tokens[32];
cj5_result r = cj5_parse(input, strlen(input), tokens, 32, NULL);

if(r.error != CJ5_ERROR_NONE) {
    if(r.error == CJ5_ERROR_OVERFLOW) {
        // use r.num_tokens to determine the actual token count
        printf("Error: line: %d, col: %d\n", r.error_line, r.error);
    }
} else {
    // lookup member of the root object
    unsigned int idx = 0;
    cj5_error_code err = cj5_find(&r, &idx, "test");
    if(err == CJ5_ERROR_NONE && tokens[idx].type == CJ5_TOKEN_NUMBER) {
        // parse number-token as floating-point
        double num;
        err = cj5_get_number(&r, idx, &num);
        if(err == CJ5_ERROR_NONE) {
            printf("parsed %d\n", num);
        }
    }
}
```

**NOTE**: If the number of parsed tokens exceeds the provided ones, the parser
          doesn't return immediately with an error. Instead it parses the JSON
          to the end, counts all needed tokens and returns with an
          CJ5_ERROR_OVERFLOW, so the user can choose to reparse the json with
          enough pre-allocated token.

**NOTE**: Optionally, if the root object finishes before the end of the input,
          then success is returned without considering what follows afterwards.
          This allows a JSON5-string to be parsed inside a "bigger context".

## Links
- [jsmn](https://github.com/zserge/jsmn): Fast and lightweight JSON parser/tokenizer
- [cj5](https://github.com/septag/cj5): Single header JSON5 parser/tokenizer derived from jsmn