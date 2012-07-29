/* varnam.h
 *
 * Copyright (C) Navaneeth.K.N
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */


#ifndef VARNAM_API_H_INCLUDED_093510
#define VARNAM_API_H_INCLUDED_093510

#include <stdlib.h>
#include "vtypes.h"
#include "util.h"
#include "varray.h"

/**
 * Initializes the varnam library.
 *
 * scheme_file    - Full path to the varnam scheme file
 * handle         - If successfull, a valid instance of varnam will
 point here. This needs to passed to all the functions in
 varnam library
 * msg            - If any error happens, this will contain the error message.
 User has to free this
 *
 * RETURN
 *
 * VARNAM_SUCCESS       - Successfully initialized
 * VARNAM_ARGS_ERROR    - When mandatory arguements are NULL
 * VARNAM_MEMORY_ERROR  - No sufficient memory to initialize
 * VARNAM_STORAGE_ERROR - Errors related to underlying file
 *
 **/
VARNAM_EXPORT extern int
varnam_init(const char *scheme_file, varnam **handle, char **msg);

VARNAM_EXPORT extern int
varnam_register_renderer(
    varnam *handle,
    const char *scheme_id,
    int (*tl)(varnam *handle, vtoken *previous, vtoken *current,  strbuf *output),
    int (*rtl)(varnam *handle, vtoken *previous, vtoken *current,  strbuf *output)
);

/**
 * Configure varnam library.
 *
 * handle - Valid varnam instance
 * type   - One of VARNAM_CONFIG_XXX
 * args   - Depends on the specified VARNAM_CONFIG_XXX
 *
 * NOTES
 *
 * This function won't persist configuration options permanantly. It will reset
 * back to the default configuration when varnam_init() is called next time.
 *
 * Following configuration options are available.
 *
 * VARNAM_CONFIG_USE_DEAD_CONSONANTS
 *   This will make varnam_create_token() to infer dead consonants and persist that rather
 *   than storing consonants. This option is set by default when varnam initializes.
 *   Eg : varnam_config(handle, VARNAM_CONFIG_USE_DEAD_CONSONANTS, 0) - Turns this option off
 *        varnam_config(handle, VARNAM_CONFIG_USE_DEAD_CONSONANTS, 1) - Turns this option on
 *
 * VARNAM_CONFIG_IGNORE_DUPLICATE_TOKEN
 *   This will make varnam_create_token() to ignore duplicate tokens. Varnam will issue a warning
 *   log message when duplicates are detected.
 *
 * VARNAM_CONFIG_ENABLE_SUGGESTIONS
 *   If this option is set, varnam_transliterate() will return possible suggestions from the words
 *   known to varnam. This configuration option takes a file which contains all know words
 *   Eg: varnam_config(handle, VARNAM_CONFIG_ENABLE_SUGGESTIONS, "/home/user/.words") - Use words from the file
 *       varnam_config(handle, VARNAM_CONFIG_ENABLE_SUGGESTIONS, NULL) - Turn off suggestions
 *
 * RETURN
 *
 * VARNAM_SUCCESS         - Successfull operation
 * VARNAM_ARGS_ERROR      - Invalid handle
 * VARNAM_INVALID_CONFIG  - Invalid configuration option
 * VARNAM_ERROR           - All other errors
 **/
VARNAM_EXPORT extern int
varnam_config(varnam *handle, int type, ...);

/**
 * Creates a token
 *
 * handle     - Valid varnam instance
 * pattern    - Lookup pattern
 * value1     - Primary replacement
 * value2     - Alternative replacement (Optional)
 * token_type - One among VARNAM_TOKEN_XXX
 * match_type - Either VARNAM_MATCH_EXACT or VARNAM_MATCH_POSSIBILITY
 * buffered   - Setting TRUE will enable buffering. If set to TRUE,
 varnam_flush() has to be called to flush buffers.
 *
 * NOTES
 *
 * Turning on buffering improves performance as it delays disk writes. This is
 * helpful when creating large number of tokens in a tight loop.
 *
 *
 * RETURN
 *
 * VARNAM_SUCCESS        - On successful execution.
 * VARNAM_ARGS_ERROR     - If any of the required arguments are empty, exceeds length or invalid
 * VARNAM_STORAGE_ERROR  - Any error related to writing to disk.
 * VARNAM_ERROR          - Other errors
 **/
VARNAM_EXPORT extern int varnam_create_token(
    varnam *handle,
    const char *pattern,
    const char *value1,
    const char *value2,
    const char *tag,
    int token_type,
    int match_type,
    int buffered
    );

/**
 * Auto generate consonant-vowel combinations
 *
 * Given vowels "a", "e", "i" and a consonant "k", this function generates
 * tokens "ka", "ke", "ki"
 *
 * handle - Valid varnam instance
 *
 * RETURN
 *
 * VARNAM_SUCCESS        - On successful execution
 * VARNAM_ARGS_ERROR     - Invalid handle
 * VARNAM_ERROR          - Any other errors. Check varnam_last_error()
 *
 **/
VARNAM_EXPORT extern int
varnam_generate_cv_combinations(varnam* handle);

VARNAM_EXPORT extern int
varnam_transliterate(varnam *handle, const char *input, varray **output);

VARNAM_EXPORT extern int
varnam_reverse_transliterate(varnam *handle, const char *input, char **result);

/**
 * Varnam will learn the supplied word. It will also learn all possible ways to write
 * the supplied word.
 *
 * handle            - Valid varnam instance
 * word              - word to learn
 *
 * RETURN
 *
 * VARNAM_SUCCESS    - Upon successful execution
 * VARNAM_ARGS_ERROR - Handle or word is NULL
 * VARNAM_ERROR      - Any other errors
 **/
VARNAM_EXPORT extern int
varnam_learn(varnam *handle, const char *word);

/**
 * Varnam will learn words from the supplied file
 *
 * handle            - Valid varnam instance
 * filepath          - file to read from
 * status            - Instance of vlearn_status struct. Varnam will write status of learning to this.
 * callback          - Function which will be invoked for each word
 * object            - Any user data which will be passed in while calling callback
 *
 * RETURN
 *
 * VARNAM_SUCCESS    - Upon successful execution
 * VARNAM_ARGS_ERROR - Handle or word is NULL
 * VARNAM_ERROR      - Any other errors
 **/
VARNAM_EXPORT extern int
varnam_learn_from_file(varnam *handle,
                       const char *filepath,
                       vlearn_status *status,
                       void (*callback)(varnam *handle, const char *word, int status, void *object),
                       void *object);

/**
 * Set scheme details. This will overwrite any scheme details set before
 *
 * handle            - Valid varnam instance
 * language_code     - ISO 639-1 standard two letter language code
 * identifier        - Unique identifier used to identify this scheme
 * display_name      - Friendly name for the scheme
 * author            - Author
 * compiled_date     - Date on which the compilation happened.
 *
 * RETURN
 *
 * VARNAM_SUCCESS    - Upon successful execution
 * VARNAM_ARGS_ERROR - If any arguements are not supplied.
 * VARNAM_ERROR      - Any other error
 *
 **/
VARNAM_EXPORT extern int
varnam_set_scheme_details(
    varnam *handle,
    const char *language_code,
    const char *identifier,
    const char *display_name,
    const char *author,
    const char *compiled_date
    );

/**
 * Returns the language code for the current scheme.
 * See varnam_set_scheme_details() to set the values
 *
 **/
VARNAM_EXPORT extern const char*
varnam_get_scheme_language_code(varnam *handle);

/**
 * Returns the identifier for the current scheme.
 * See varnam_set_scheme_details() to set the values
 *
 **/
VARNAM_EXPORT extern const char*
varnam_get_scheme_identifier(varnam *handle);

/**
 * Returns the friendly display name for the current scheme.
 * See varnam_set_scheme_details() to set the values
 *
 **/
VARNAM_EXPORT extern const char*
varnam_get_scheme_display_name(varnam *handle);

/**
 * Returns the author for the current scheme.
 * See varnam_set_scheme_details() to set the values
 *
 **/
VARNAM_EXPORT extern const char*
varnam_get_scheme_author(varnam *handle);

/**
 * Returns the compiled date for the current scheme.
 * See varnam_set_scheme_details() to set the values
 *
 **/
VARNAM_EXPORT extern const char*
varnam_get_scheme_compiled_date(varnam *handle);

VARNAM_EXPORT extern int
varnam_set_metadata(
    varnam *handle,
    const char *
    );

/**
 * Returns error message for the most recent failed call to varnam API functions.
 *
 **/
VARNAM_EXPORT extern const char*
varnam_get_last_error(varnam *handle);

/**
 * Retrieves all tokens matching the supplied token type
 *
 * handle     - Valid varnam instance
 * token_type - One among VARNAM_TOKEN_XXX
 * tokens     - Output will be written here. This will be an array of tokens.
 *
 * RETURN
 *
 * VARNAM_SUCCESS    - On successful execution
 * VARNAM_ARGS_ERROR - Invalid handle or incorrect token type
 * VARNAM_ERROR      - Any other errors
 * */
VARNAM_EXPORT extern int
varnam_get_all_tokens(
    varnam *handle,
    int token_type,
    varray **tokens
    );

/**
 * Enable logging.
 *
 * handle   - A valid varnam instance
 * log_type - Either VARNAM_LOG_DEFAULT or VARNAM_LOG_DEBUG
 * callback - Actual function that does the logging
 *
 * USAGE
 *
 * varnam_enable_logging (handle, VARNAM_LOG_DEFAULT, func) - Enables default logging
 * varnam_enable_logging (handle, VARNAM_LOG_DEBUG, func)    - Enables debug level logging
 * varnam_enable_logging (handle, VARNAM_LOG_DEFAULT, NULL) - Disables logging
 *
 * Logging is hierarchical. So enabling debug level logging will generate all the log messages
 * varnam is writing. Default logging level will not produce debug level messages
 **/
VARNAM_EXPORT extern int varnam_enable_logging(
    varnam *handle,
    int log_type,
    void (*callback)(const char*)
    );

/**
 * Writes changes in the buffer to the disk. Calling this function when no buffered data is available will be a no-op
 *
 * handle - A valid varnam instance
 *
 * NOTES
 *
 * Usually this is called after completing a buffered operation like, varnam_create_token().
 *
 * RETURN
 *
 * VARNAM_SUCCESS       - On successfull execution
 * VARNAM_ARGS_ERROR    - When handle is invalid
 * VARNAM_STORAGE_ERROR - Errors related to writing to disk
 **/
VARNAM_EXPORT extern int varnam_flush_buffer(
    varnam *handle
    );

VARNAM_EXPORT extern int varnam_get_info(
    varnam *handle,
    bool detailed,
    vinfo **info
    );

VARNAM_EXPORT extern int
varnam_destroy(varnam *handle);

#endif
