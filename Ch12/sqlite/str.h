/* str.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef STR
#define STR
#include <stdlib.h>
#include <ctype.h>

#define STR_LOG 0
#define STR_DEBUG 0

// Structure to built a stack of allocated buffers.
typedef struct Str Str;
struct Str {
  char   *s;    // C string
  Str    *up;   // one level higher in the stack
  Str    *down; // one level deeper in the stack
  size_t size;  // size of allocated block
  Str    **stack_p;
  };

/*
 ******************************************** Allocation and release **********
 */
// Macro to report a catastrophic error and abort the program
#define STR_crash(e) str_crash(e, __FILE__, __LINE__)

// Macro to be executed first.
#define STR_setup {       \
  Str *str_stack = NULL;

// Macros to list all the strings on a stack
#define STR_list() str_list(str_stack)
#define STR_list_str(str) str_list(str)

// Macros to obtain a pointer to a new Str and initialize it with an
// existing C string. s can be NULL, in which case the new Str is
// returned uninitialized.
#define STR_new(s) str_new(&str_stack, 0, s, '\0')
#define STR_new_str(str, s) str_new(&(str), 0, s, '\0')

// Macros to obtain a pointer to a new Str with a new C string of a given
// length. If s is NULL, the new string is filled with spaces. Otherwise,
// s is copied in part or in total (possibly padded) to the newly
// allocated string.
#define STR_newn(len, s, c) str_new(&str_stack, len, s, c)
#define STR_newn_str(str, len, s, c) str_new(&(str), len, s, c)

// Macro to release a Str.
#define STR_release(str) str_release(&(str))

// Macros to release all Str.
#define STR_release_all() str_release_all(str_stack)
#define STR_release_all_str(str) str_release_all(str)

// Macro to be executed last
#define STR_dismantle          \
  str_release_all(str_stack);  \
  }

void str_crash(char *e, char *file, int line);
void str_list(Str *str);

// str_new() allocates a Str and, possibly, space for a C string.
// If len is 0, the s component of the new Str is set to the given C string.
// If len > 0, an array of len+1 chars is allocated and, if the third argument
// is not NULL, it is copied in part or in total (possibly padded with spaces)
// to the newly allocated C string.
Str *str_new(Str **str_p, size_t len, char *s, char pad);

// str_release() frees back to the C heap a Str with the associated C string
// (if the C string was allocated together with the Str).
void str_release(Str **str);

// str_release_all() frees back to the C heap all allocated Str and C strings.
void str_release_all(Str *str);

/*
 ******************************************************** Formatting **********
 */
#define STR_printf(fmt, ...) str_printf(&str_stack, fmt, __VA_ARGS__)
#define STR_printf_str(str, fmt, ...) str_printf(&(str), fmt, __VA_ARGS__)

Str *str_printf(Str **str_p, char *fmt, ...);

/*
 ************************************************************** Info **********
 */
/*
#define STR_display(str) {                                               \
    printf("%p:\n    up=%p down=%p size=%zu stack=%p\n",                 \
    str, (str)->up, (str)->down, (str)->size, *(str)->stack_p);          \
    printf("    s(%lu)=\"%s\"\n", (str)->size - sizeof(Str), (str)->s);  \
    }
*/
#define STR_display(str) {                                             \
    Str *sstr = str;                                                   \
    printf("%p:\n    up=%p down=%p size=%zu stack=%p\n",               \
    sstr, sstr->up, sstr->down, sstr->size, *sstr->stack_p);           \
    printf("    s(%lu)=\"%s\"\n", sstr->size - sizeof(Str), sstr->s);  \
    }

/*
#define STR_len(str) ((str != NULL && (str)->s != NULL) ? strlen((str)->s) : 0)
*/
// STR_len() defines sstr_l instead of sstr like all other macros
// because it is used within other macros.
#define STR_len(str) ({                                             \
    Str *sstr_l = str;                                              \
    (sstr_l != NULL && sstr_l->s != NULL) ? strlen(sstr_l->s) : 0;  \
    })

/*
#define STR_len_allocated(str) ((str != NULL) ? ((str)->size-sizeof(Str)) : 0)
*/
#define STR_len_allocated(str) ({                   \
    Str *sstr = str;                                \
    (sstr != NULL) ? (sstr->size-sizeof(Str)) : 0;  \
    })
#if STR_DEBUG
/*
#define STR_char(str, k)                                          \
    ((str)->s != NULL && (strlen((str)->s) > k && k >= 0)         \
    ? (str)->s[k]                                                 \
    : (STR_crash("STR_char: Character outside a string"), '\0'))
*/
#define STR_char(str, k) ({                                       \
    Str *sstr = str;                                              \
    sstr->s != NULL && (strlen(sstr->s) > k && k >= 0)            \
    ? sstr->s[k]                                                  \
    : (STR_crash("STR_char: Character outside a string"), '\0');  \
    })
#else
#define STR_char(str, k) (str)->s[k]
#endif
#define STR_string(str) ((str)->s)

/*
 ************************************************************ Update **********
 */
/*
#define STR_set_char(str, k, c) {                                            \
    if((str)->s != NULL && strlen((str)->s) > k && k >= 0) (str)->s[k] = c;  \
    else STR_crash("STR_set_char: character outside a string");              \
    }
    */
#define STR_set_char(str, k, c) {                                         \
    Str *sstr = str;                                                      \
    if(sstr->s != NULL && strlen(sstr->s) > k && k >= 0) sstr->s[k] = c;  \
    else STR_crash("STR_set_char: character outside a string");           \
    }
#define STR_copy_string(str, st, pad) str_copy_string(str, st, pad)
#define STR_toupper(str) str_to_upper_lower(str, &toupper)
#define STR_tolower(str) str_to_upper_lower(str, &tolower)
#define STR_clean_up(str) str_clean_up(str)
#if STR_DEBUG
#define STR_remove(str, from, before) str_remove(str, from, before)
#define STR_remove_from(str, from) str_remove(str, from, STR_len(str))
#define STR_remove_before(str, before) str_remove(str, 0, before)
#else
/*
#define STR_remove(str, from, before)               \
    strcpy((str)->s + (from), (str)->s + (before))
    */
#define STR_remove(str, from, before) {            \
    Str *sstr = str;                               \
    strcpy(sstr->s + (from), sstr->s + (before));  \
    }
#define STR_remove_from(str, from) { *((str)->s + (from)) ='\0'; }
/*
#define STR_remove_before(str, before) strcpy((str)->s, (str)->s + (before))
*/
#define STR_remove_before(str, before) {  \
    Str *sstr = str;                      \
    strcpy(sstr->s, sstr->s + (before));  \
    }
#endif

// It fits the input string to the output Str by truncation and padding.
size_t str_copy_string(Str *str, char *s, char pad);

void str_to_upper_lower(Str *str, int (*f)(int));
void str_clean_up(Str *str);

void str_remove(Str *str, size_t from, size_t before);

/*
 ************************************************************ Search **********
 */

// Search for characters, case sensitive or not, from the beginning of a
// string (forward), from its end (backward), or from any position within
// the string (forward or backward).
#if STR_DEBUG
#define STR_first_c(str, c) str_first_c(str, c, 0, 0)
#define STR_first_c_from(str, c, from) str_first_c(str, c, 0, from)
#else
#define STR_first_c(str, c) strchr((str)->s, c)
#define STR_first_c_from(str, c, from) strchr((str)->s + from, c)
#endif
#define STR_first_cc(str, c) str_first_c(str, c, 1, 0)
#define STR_first_cc_from(str, c, from) str_first_c(str, c, 1, from)
#define STR_last_c(str, c) str_last_c(str, c, 0, STR_len(str))
#define STR_last_c_before(str, c, before) str_last_c(str, c, 0, before)
#define STR_last_cc(str, c) str_last_c(str, c, 1, STR_len(str))
#define STR_last_cc_before(str, c, before) str_last_c(str, c, 1, before)

char *str_first_c(Str *str, char c, int ignore, size_t from);
char *str_last_c(Str *str, char c, int ignore, size_t before);

// Search for substrings from the beginning of a string (forward), from its end
// (backward), or from any position within the string (forward or backward).
#if STR_DEBUG
#define STR_first(str, s) str_first(str, s, 0)
#define STR_first_from(str, s, from) str_first(str, s, from)
#else
/*
#define STR_first(str, ss)                                            \
    (((ss) == NULL || strlen(ss) == 0) ? NULL : strstr((str)->s, ss))
*/
#define STR_first(str, ss) ({                                          \
    char *sss = ss;                                                   \
    (sss == NULL || strlen(sss) == 0) ? NULL : strstr((str)->s, sss);  \
    })
/*
#define STR_first_from(str, ss, from)                                        \
    (((ss) == NULL || strlen(ss) == 0) ? NULL : strstr((str)->s + from, ss))
*/
#define STR_first_from(str, ss, from) ({                                      \
    char *sss = ss;                                                           \
    (sss == NULL || strlen(sss) == 0) ? NULL : strstr((str)->s + from, sss);  \
    })
#endif
/*
#define STR_last(str, s) str_last(str, s, STR_len(str))
*/
#define STR_last(str, s) ({            \
    Str *sstr = str;                   \
    str_last(sstr, s, STR_len(sstr));  \
    })
#define STR_last_before(str, s, before) str_last(str, s, before)

char *str_first(Str *str, char *s, size_t from);
char *str_last(Str *str, char *s, size_t before);

/*
 *********************************************************** Replace **********
 */

// Replace all occurrences of a character with another one, case sensitive
// or not.
#define STR_replace_c(str, old, new) str_replace_c(str, old, new, 0)
#define STR_replace_cc(str, old, new) str_replace_c(str, old, new, 1)

void str_replace_c(Str *str, char old, char new, int keep);

// Replace all occurrences of a substring with another substring provided
// it is not longer than the original one.
#define STR_replace(str, old, new) str_replace(str, old, new)

void str_replace(Str *str, char *old, char *new);

/*
 ************************************************* Extract & Combine **********
 */

// Extract a substring from an Str.
#if STR_DEBUG
#define STR_sub(str, from, before) str_sub(str, from, before)
#define STR_sub_from(str, from) str_sub(str, from, STR_len(str))
#define STR_sub_before(str, before) str_sub(str, 0, before)
#else
/*
#define STR_sub(str, from, before)                             \
    str_new(&(str), (before) - (from), (str)->s + (from), ' ')
    */
#define STR_sub(str, from, before) ({                                 \
    Str *sstr = str; \
    str_new(&sstr, (before) - (from), sstr->s + (from), ' ');\
    })
/*
#define STR_sub_from(str, from)                                    \
    str_new(&(str), STR_len(str) - (from), (str)->s + (from), ' ')
*/
#define STR_sub_from(str, from) ({                                    \
    Str *sstr = str; \
    str_new(&sstr, STR_len(str) - (from), sstr->s + (from), ' '); \
    })
/*
#define STR_sub_before(str, before)          \
    str_new(&(str), (before), (str)->s, ' ')
*/
#define STR_sub_before(str, before) ({              \
    Str *sstr = str; \
    str_new(&sstr, (before), sstr->s, ' '); \
    })
#endif

// Make an exact copy of an Str.
//#define STR_clone(str) str_sub(str, 0, STR_len(str))
/*
#define STR_clone(str) str_new(&str, STR_len(str), STR_string(str), ' ')
*/
#define STR_clone(str) ({                                \
  Str *sstr = str;                                       \
  str_new(&sstr, STR_len(sstr), STR_string(sstr), ' ');  \
  })

// Create a new Str by concatenating two or more existing Str.
// It causes a compilation error if you try to concatenate 0 strings.
// But why would you do that?   :-)
#define STR_concat(...) str_concat(&str_stack, __VA_ARGS__, NULL)
#define STR_concat_str(str0, ...) str_concat(&(str0), __VA_ARGS__, NULL)

Str *str_sub(Str *str, size_t from, size_t before);
Str *str_concat(Str **str0, ...);

#endif
