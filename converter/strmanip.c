#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include "strmanip.h"

#pragma comment(lib, "pcre.lib")

char *preg_get_replacement(const char *replacement, const char *src, const regmatch_t *matches, int nsub, int *rlen_out)
{
	char *s;
	int i, len = 0;
	int rlen;

	rlen = preg_get_replacement_length(replacement, matches, nsub);
	if (rlen_out != NULL)
		*rlen_out = rlen;

	s = malloc(rlen + 1);
	if (s == NULL)
		return NULL;
	while (*replacement != '\0')
	{
		if (*replacement == '$' && *(replacement + 1) != '\0')
		{
			if (*(replacement + 1) == '$')
			{
				s[len] = '$';
				len++;
				replacement += 2;
				continue;
			}

			i = *(replacement + 1) - '0';
			if (i >= 0 && i <= 9 && i <= nsub)
			{
				memcpy(s + len, src + matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so);
				len += matches[i].rm_eo - matches[i].rm_so;
				replacement += 2;
				continue;
			}
		}
		s[len] = *replacement;
		len++;
		replacement++;
	}
	s[len] = '\0';
	return s;
}

int preg_get_replacement_length(const char *replacement, const regmatch_t *matches, int nsub)
{
	int i, len = 0;

	while (*replacement != '\0')
	{
		if (*replacement == '$' && *(replacement + 1) != '\0')
		{
			if (*(replacement + 1) == '$')
			{
				len++;
				replacement += 2;
				continue;
			}

			i = *(replacement + 1) - '0';
			if (i >= 0 && i <= 9 && i <= nsub)
			{
				len += matches[i].rm_eo - matches[i].rm_so;
				replacement += 2;
				continue;
			}
		}
		len++;
		replacement++;
	}
	return len;
}

/* searches subject for matches to pattern and replaces them with replacement */
// (supports $0~$9 and $$)
char *preg_replace(const regex_t *pattern, const char *replacement, const char *subject)
{
	char *new_str;
	char *new_replacement;
	int i, len = 0;
	int rlen;
	int offset;
	regmatch_t matches[10];

	// calculate the length of the final string
	offset = 0;
	while (regexec(pattern, subject + offset, 10, matches, 0) == 0)
	{
		rlen = preg_get_replacement_length(replacement, matches, pattern->re_nsub);
		len += matches[0].rm_so + rlen; // mismatched length + replaced length
		offset += matches[0].rm_eo; // prepare for the next match
	}
	if (offset == 0)
		return NULL; // exit if mismatched
	len += strlen(subject + offset); // the remaining length

	// build the final string
	new_str = malloc(len + 1);
	if (new_str == NULL)
		return NULL;
	i = 0;
	offset = 0;
	while (regexec(pattern, subject + offset, 10, matches, 0) == 0)
	{
		if (matches[0].rm_so > 0)
		{
			memcpy(new_str + i, subject + offset, matches[0].rm_so);
			i += matches[0].rm_so;
		}
		new_replacement = preg_get_replacement(replacement, subject + offset, matches, pattern->re_nsub, &rlen);
		if (new_replacement != NULL)
		{
			if (rlen > 0)
			{
				memcpy(new_str + i, new_replacement, rlen);
				i += rlen;
			}
			free(new_replacement);
		}
		offset += matches[0].rm_eo;
	}
	if (i < len)
		memcpy(new_str + i, subject + offset, len - i);
	new_str[len] = '\0';
	return new_str;
}
 
char *str_replace(const char *search, const char *replace, char *subject, int bufsize)
{
	char *p;
	int i, j = 0, n = 0;
	int len, slen, rlen, tlen;
	
	// check arguments
	if (subject == NULL)
		return NULL;
	tlen = (int)strlen(subject);
	if (bufsize == -1)
	{
		// make sure subject is writable
		p = malloc(tlen + 1);
		if (p == NULL)
			return NULL;
		memcpy(p, subject, tlen + 1);
		subject = p;
	}
	if (search == NULL || search[0] == '\0')
		return subject;
	slen = (int)strlen(search);
	if (replace == NULL)
		replace = "";
	rlen = (int)strlen(replace);
 
	// find out how many replacements are needed
	for (i = 0; subject[i] != '\0'; i++)
	{
		if (subject[i] != search[j])
		{
			// character mismatch
			j = 0;
		}
		if (subject[i] == search[j])
		{
			j++;
			if (j == slen)
			{
				// pattern match
				subject[i + 1 - j] = '\0'; // front
				subject[i] = '\0'; // rear
				j = 0;
				n++;
			}
		}
	}
	if (n == 0)
		return subject;
 
	// calculate the length of the new string and check buffer size
	len = tlen + n * (rlen - slen);
	if (bufsize == -1)
	{
		if (len > tlen)
		{
			// increase buffer size
			p = realloc(subject, len + 1);
			if (p == NULL)
			{
				free(subject);
				return NULL;
			}
			subject = p;
		}
	}
	else if (bufsize < len + 1)
		return NULL;
 
	// replace substrings
	if (len <= tlen)
	{
		i = 0; // for read
		j = 0; // for write
		while (i < tlen)
		{
			if (subject[i] == '\0')
			{
				memcpy(subject + j, replace, rlen);
				i += slen;
				j += rlen;
			}
			else
			{
				subject[j] = subject[i];
				i++;
				j++;
			}
		}
	}
	else
	{
		i = tlen - 1;
		j = len - 1;
		while (i >= 0)
		{
			if (subject[i] == '\0')
			{
				memcpy(subject + j + 1 - rlen, replace, rlen);
				i -= slen;
				j -= rlen;
			}
			else
			{
				subject[j] = subject[i];
				i--;
				j--;
			}
		}
	}
	subject[len] = '\0';
	return subject;
}
 
int trim(char *s)
{
	int i, j = -1, len = 0;
 
	for (i = 0; s[i] != '\0'; i++)
	{
		if (j == -1 && s[i] != ' ' && s[i] != '\t')
			j = 0;
		if (j != -1)
		{
			s[j] = s[i];
			j++;
			if (s[i] != ' ' && s[i] != '\t' && s[i] != '\r' && s[i] != '\n')
				len = j;
		}
	}
	s[len] = '\0';
	return len;
}