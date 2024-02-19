#pragma once
#define CONVERTER_MAX_LINE    10240
#define CONVERTER_MAX_NAME    256
#define CONVERTER_MAX_CLASS   1024000
#define CONVERTER_MAX_STRLIST 102400

struct converter_state
{
	char line[CONVERTER_MAX_LINE];
	char *line_tokens[CONVERTER_MAX_LINE / 10];
	char line_tokens_buffer[CONVERTER_MAX_LINE];

	int in_extern_c;

	int in_enum;
	char enum_name[CONVERTER_MAX_NAME];
	char enum_values[CONVERTER_MAX_STRLIST];

	int in_struct;
	char struct_name[CONVERTER_MAX_NAME];

	int in_class;
	char class_name[CONVERTER_MAX_NAME];
	char class_enums[CONVERTER_MAX_CLASS];
	char class_members[CONVERTER_MAX_CLASS];
	char indent[20];

	int in_function;
	char function_name[CONVERTER_MAX_NAME];
	char function_params[CONVERTER_MAX_STRLIST];

	FILE *fp_out;
	regex_t regexps[4];
};

#define converter_assert(exp, ret)				do { \
													assert(exp); \
													if (!(exp)) \
														return ret; \
												} while (0)
#define converter_clear_strlist(buffer)         memset((buffer), 0, 2)
#define converter_get_input_filepath(filename)  converter_get_filepath("input_files", filename)
#define converter_get_output_filepath(filename) converter_get_filepath("output_files", filename)

int converter_add_to_strlist(char *buffer, int bufsize, const char *value);
void converter_convert(const char *filename);
void converter_free_regexps(struct converter_state *state);
const char *converter_get_filepath(const char *folder, const char *filename);
int converter_get_strlist_usage(const char *buffer);
const char *converter_get_type_name(const char **line_tokens, const char *keyword, int *has_curly_brace);
void converter_init_regexps(struct converter_state *state);
int converter_is_extern_c(const char **line_tokens);
int converter_is_identifier(const char *str);
int converter_printf(struct converter_state *state, const char *fmt, ...);
int converter_process_class_members(struct converter_state *state);
int converter_process_enum_members(struct converter_state *state);
int converter_process_special_cases(struct converter_state *state);
int converter_process_struct_members(struct converter_state *state);
int converter_process_type_names(struct converter_state *state);
int converter_process_variables(struct converter_state *state);
int converter_search_strlist(const char *buffer, const char *value);
int converter_split(char *buffer, char **tokens, int tokens_arrlen);
int converter_split_line(struct converter_state *state);
void converter_take_enum_comments(struct converter_state *state);
void converter_unindent(struct converter_state *state);
