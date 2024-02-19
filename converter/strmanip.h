#pragma once
char *preg_get_replacement(const char *replacement, const char *src, const regmatch_t *matches, int nsub, int *rlen_out);
int preg_get_replacement_length(const char *replacement, const regmatch_t *matches, int nsub);
char *preg_replace(const regex_t *pattern, const char *replacement, const char *subject);
char *str_replace(const char *search, const char *replace, char *subject, int bufsize);
char *substr_replace(char *string, const char *replacement, int start, int length, int bufsize);
