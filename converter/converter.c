#include <assert.h>
#include <ctype.h>
#include <direct.h>
#include <io.h>
#include <regex.h>
#include <stdio.h>
#include <shlwapi.h>
#include "strmanip.h"
#include "converter.h"

#pragma comment(lib, "Shlwapi.lib")

int converter_add_to_strlist(char *buffer, int bufsize, const char *value)
{
	int len, usage;

	if (converter_search_strlist(buffer, value))
		return 0;

	usage = converter_get_strlist_usage(buffer);
	len = (int)strlen(value) + 1;
	converter_assert(usage + len <= bufsize, -1);

	memcpy(buffer + usage - 1, value, len);
	buffer[usage - 1 + len] = '\0';
	return 1;
}

void converter_convert(const char *filename)
{
	struct converter_state *state = NULL;
	FILE *fp = NULL;

	state = malloc(sizeof(struct converter_state));
	if (state == NULL)
		goto end;
	memset(state, 0, sizeof(struct converter_state));
	converter_init_regexps(state);

	fopen_s(&fp, converter_get_input_filepath(filename), "r");
	if (fp == NULL)
		goto end;
	fopen_s(&state->fp_out, converter_get_output_filepath(filename), "w");
	if (state->fp_out == NULL)
		goto end;

	while (fgets(state->line, CONVERTER_MAX_LINE, fp))
	{
		state->indent[0] = '\0';
		if (state->in_class)
			converter_unindent(state);

		strcpy_s(state->line_tokens_buffer, CONVERTER_MAX_LINE, state->line);
		trim(state->line_tokens_buffer);
		converter_split(state->line_tokens_buffer, state->line_tokens, _countof(state->line_tokens));

		if (state->line_tokens[0] != NULL)
		{
			if (converter_process_special_cases(state))
				continue;
			if (converter_process_type_names(state))
				continue;
			if (converter_process_variables(state))
				continue;
			if (converter_process_enum_members(state))
				continue;
			if (converter_process_struct_members(state))
				continue;
		}

		if (converter_process_class_members(state))
			continue;

		converter_printf(state, "%s", state->line);
	}

end:
	if (fp != NULL)
		fclose(fp);
	if (state != NULL)
	{
		converter_free_regexps(state);
		if (state->fp_out != NULL)
			fclose(state->fp_out);
		free(state);
	}
}

void converter_free_regexps(struct converter_state *state)
{
	int i;

	for (i = 0; i < _countof(state->regexps); i++)
		regfree(&state->regexps[i]);
}

const char *converter_get_filepath(const char *folder, const char *filename)
{
	static char output_filepath[MAX_PATH];

	GetModuleFileNameA(NULL, output_filepath, MAX_PATH);
	PathRemoveFileSpecA(output_filepath);
	PathCombineA(output_filepath, output_filepath, folder);

	// create the folder if it does not exist
	if (_access(output_filepath, 0) != 0)
		_mkdir(output_filepath);

	PathCombineA(output_filepath, output_filepath, filename);
	return output_filepath;
}

int converter_get_strlist_usage(const char *buffer)
{
	const char *p = buffer;

	while (*p != '\0')
		p += strlen(p) + 1;
	return (int)(p - buffer) + 1;
}

const char *converter_get_type_name(const char **line_tokens, const char *keyword, int *has_curly_brace)
{
	static char name[CONVERTER_MAX_NAME];
	int len;

	if (strcmp(line_tokens[0], keyword) == 0)
	{
		if (line_tokens[1] != NULL && strcmp(line_tokens[1], "__declspec(novtable)") == 0)
			line_tokens++;

		if (line_tokens[1] != NULL && strcmp(line_tokens[1], "{") != 0)
		{
			strcpy_s(name, CONVERTER_MAX_NAME, line_tokens[1]);
			line_tokens++;
		}
		else
			name[0] = '\0';

		*has_curly_brace = 0;
		if (line_tokens[1] != NULL && strcmp(line_tokens[1], "{") == 0)
			*has_curly_brace = 1;
		else
		{
			len = strlen(name);
			if (len != 0 && name[len - 1] == ';')
			{
				*has_curly_brace = -1;
				name[len - 1] = '\0';
			}
		}
		return name;
	}
	return NULL;
}

void converter_init_regexps(struct converter_state *state)
{
	int ret;

	ret = regcomp(&state->regexps[0], "virtual ([\\w_]+) ([\\w_]+ )?([\\w_]+)\\(([^\\)]*)\\)( = 0)?;", 0);
	ret = regcomp(&state->regexps[1], "const [\\w_]+ ([\\w_]+) = ([^;]+);", 0);
	ret = regcomp(&state->regexps[2], "([\\w_]+)(?<!return) [\\w_]+;", REG_NOSUB);
	ret = regcomp(&state->regexps[3], "^\\s*(\\/\\/|\\n?$)", REG_NOSUB);
	assert(ret == 0);
}

int converter_is_extern_c(const char **line_tokens)
{
	if (line_tokens[0] != NULL && line_tokens[1] != NULL && line_tokens[2] != NULL)
	{
		if (strcmp(line_tokens[0], "extern") == 0 && strcmp(line_tokens[1], "\"C\"") == 0 && strcmp(line_tokens[2], "{") == 0)
			return 1;
	}
	return 0;
}

int converter_is_identifier(const char *str)
{
	int i;
	unsigned char c;

	if (str == NULL || str[0] == '\0')
		return 0;
	for (i = 0; str[i] != '\0'; i++)
	{
		c = str[i];
		if (!isalnum(c) && c != '_')
			return 0;
	}
	return 1;
}

int converter_printf(struct converter_state *state, const char *fmt, ...)
{
	int offset, size;
	int ret;
	va_list list;

	va_start(list, fmt);
	if (state->in_class)
	{
		if (state->in_enum)
		{
			offset = strlen(state->class_enums);
			size = CONVERTER_MAX_CLASS - offset;
			ret = vsprintf_s(state->class_enums + offset, size, fmt, list);
		}
		else
		{
			offset = strlen(state->class_members);
			size = CONVERTER_MAX_CLASS - offset; // >=1 because '\0' is always present
			ret = vsprintf_s(state->class_members + offset, size, fmt, list);
		}
	}
	else
		ret = vfprintf(state->fp_out, fmt, list);
	va_end(list);
	return ret;
}

int converter_process_class_members(struct converter_state *state)
{
	if (!state->in_class)
		return 0;

	if (state->line_tokens[0] != NULL)
	{
		if (state->line_tokens[1] == NULL)
		{
			if (strcmp(state->line_tokens[0], "{") == 0)
			{
				if (state->in_class == 1)
				{
					state->in_class = 2;
					fprintf(state->fp_out, "%s", state->line);
					return 1;
				}
			}
			else if (strcmp(state->line_tokens[0], "public:") == 0 || strcmp(state->line_tokens[0], "private:") == 0 || strcmp(state->line_tokens[0], "protected:") == 0)
				return 1;
		}

		// directly print out struct members
		if (regexec(&state->regexps[2], state->line, 0, NULL, 0) == 0)
		{
			fprintf(state->fp_out, "%s%s", state->indent, state->line);
			return 1;
		}
	}
	return 0;
}

int converter_process_enum_members(struct converter_state *state)
{
	int ret;

	if (!state->in_enum)
		return 0;

	if (converter_is_identifier(state->line_tokens[0]))
	{
		ret = converter_add_to_strlist(state->enum_values, CONVERTER_MAX_STRLIST, state->line_tokens[0]);
		if (ret == 1)
			printf("add enum value %s\n", state->line_tokens[0]);
	}
	return 0;
}

int converter_process_special_cases(struct converter_state *state)
{
	if (strcmp(state->line_tokens[0], "#define") == 0 && state->line_tokens[1] != NULL)
	{
		if (strcmp(state->line_tokens[1], "GDIP_WMF_RECORD_TO_EMFPLUS(n)") == 0)
		{
			str_replace("(EmfPlusRecordType)", "(enum _EmfPlusRecordType)", state->line, CONVERTER_MAX_LINE);
			converter_printf(state, "%s", state->line);
			return 1;
		}
	}
	return 0;
}

int converter_process_struct_members(struct converter_state *state)
{
	char *s;

	if (!state->in_struct)
		return 0;

	s = preg_replace(&state->regexps[0], "$1 ($2*$3)($4);", state->line);
	if (s != NULL)
	{
		converter_printf(state, "%s", s);
		free(s);
		return 1;
	}

	return 0;
}

int converter_process_type_names(struct converter_state *state)
{
	const char *type_name;
	int has_brace;

	if (converter_is_extern_c(state->line_tokens))
	{
		state->in_extern_c = 1;
		return 1;
	}

	type_name = converter_get_type_name(state->line_tokens, "enum", &has_brace);
	if (type_name != NULL)
	{
		state->in_enum = 1;
		strcpy_s(state->enum_name, CONVERTER_MAX_NAME, type_name);

		if (state->in_class)
			converter_take_enum_comments(state);

		if (type_name[0] != '\0')
			converter_printf(state, "typedef enum _%s\n", type_name);
		else
			converter_printf(state, "enum\n");
		if (has_brace)
			converter_printf(state, "{\n");
		return 1;
	}

	type_name = converter_get_type_name(state->line_tokens, "struct", &has_brace);
	if (type_name != NULL)
	{
		state->in_struct = 1;
		strcpy_s(state->struct_name, CONVERTER_MAX_NAME, type_name);

		converter_printf(state, "typedef struct _%s\n", type_name);
		if (has_brace)
			converter_printf(state, "{\n");
		return 1;
	}

	type_name = converter_get_type_name(state->line_tokens, "class", &has_brace);
	if (type_name != NULL)
	{
		if (has_brace == -1)
			converter_printf(state, "struct _%s;\n", type_name);
		else
		{
			converter_printf(state, "typedef struct _%s\n", type_name);
			if (has_brace)
				converter_printf(state, "{\n");

			state->in_class = 1;
			strcpy_s(state->class_name, CONVERTER_MAX_NAME, type_name);
		}
		return 1;
	}
	
	if (strcmp(state->line_tokens[0], "};") == 0 && state->line_tokens[1] == NULL)
	{
		if (state->in_enum)
		{
			if (state->enum_name[0] != '\0')
				converter_printf(state, "} %s;\n", state->enum_name);
			else
				converter_printf(state, "};\n", state->enum_name);
			state->in_enum = 0;
			return 1;
		}
		else if (state->in_struct)
		{
			converter_printf(state, "} %s;\n", state->struct_name);
			state->in_struct = 0;
			return 1;
		}
		else if (state->in_class)
		{
			state->in_class = 0;
			converter_printf(state, "} %s;\n%s%s", state->class_name, state->class_enums, state->class_members);
			state->class_enums[0] = '\0';
			state->class_members[0] = '\0';
			return 1;
		}
	}
	
	if (strcmp(state->line_tokens[0], "}") == 0 && state->line_tokens[1] == NULL)
	{
		if (state->in_extern_c)
		{
			state->in_extern_c = 0;
			return 1;
		}
	}
	return 0;
}

int converter_process_variables(struct converter_state *state)
{
	char *s;

	// variables must not be defined in a C header file
	// replace them with macro definitions
	s = preg_replace(&state->regexps[1], "#define $1 ($2)", state->line);
	if (s != NULL)
	{
		converter_printf(state, "%s", s);
		free(s);
		return 1;
	}
	return 0;
}

int converter_search_strlist(const char *buffer, const char *value)
{
	if (value == NULL || value[0] == '\0')
		return 1;

	while (*buffer != '\0')
	{
		if (strcmp(buffer, value) == 0)
			return 1;
		buffer += strlen(buffer) + 1;
	}
	return 0;
}

int converter_split(char *buffer, char **tokens, int tokens_arrlen)
{
	char *context;
	char *p;
	int i;

	tokens[tokens_arrlen - 1] = NULL;
	for (i = 0; (p = strtok_s(buffer, " \t\r\n", &context)) != NULL; i++)
	{
		converter_assert(i <= tokens_arrlen - 2, i);
		tokens[i] = p;
		buffer = NULL;
	}
	tokens[i] = NULL;
	return i;
}

void converter_take_enum_comments(struct converter_state *state)
{
	char c;
	int i, j, ret;

	j = strlen(state->class_members);
	i = j - 1;
	while (i >= 0)
	{
		if (i == 0 || state->class_members[i - 1] == '\n')
		{
			// [i] is the beginning of a line
			// [j-1] is the last character of the line
			c = state->class_members[j];
			state->class_members[j] = '\0';
			ret = regexec(&state->regexps[3], state->class_members + i, 0, NULL, 0);
			state->class_members[j] = c;
			if (ret != 0)
				break; // not a comment; finish
			j = i;
		}
		i--;
	}

	// move the comment lines from class_members to class_enums
	if (state->class_members[j] != '\0')
	{
		strcat_s(state->class_enums, CONVERTER_MAX_CLASS, state->class_members + j);
		state->class_members[j] = '\0';
	}
}

void converter_unindent(struct converter_state *state)
{
	char *spaces = "    ";
	int len;

	len = strlen(spaces);
	if (strncmp(state->line, spaces, len) == 0)
	{
		strcat_s(state->indent, sizeof(state->indent), spaces);
		memmove(state->line, state->line + len, strlen(state->line) + 1 - len);
	}
}