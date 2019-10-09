struct parsed_cmd {
    char ***cmd_opt_list;
    int dim;
};

typedef struct parsed_cmd PARSED_CMD;

PARSED_CMD* parse_cmd (char *);
