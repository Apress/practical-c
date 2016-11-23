/* compare.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef CMPR
#define CMPR

int cmpr_levenshtein(char *s1, char *s2);

// Function to calculate the Levenshtein distance of two strings based on:
//en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
int cmpr_levenshtein_wiki(char *s1, char *s2);

int cmpr_transposition(char *s1, char *s2);

#endif
