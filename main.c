#include "combo.h"
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 128

int my_strtol(const char *str, long *val) {
    int ret = 0;
    char *end = NULL;

    errno = 0;
    long _val = strtol(str, &end, 10);

    if (*end != '\0') {
        ret = 1;
        goto end;
    }
    if (errno == ERANGE) {
        ret = 1;
        goto end;
    }

    *val = _val;
end:
    return ret;
}

int my_strtoul(const char *str, unsigned long *val) {
    int ret = 0;
    char *end = NULL;

    errno = 0;
    long _val = strtoul(str, &end, 10);

    if (*end != '\0') {
        ret = 1;
        goto end;
    }
    if (errno == ERANGE) {
        ret = 1;
        goto end;
    }

    *val = _val;
end:
    return ret;
}

char instance_name[128] = {0};

int parse(char *filename, char **instance, stype *c, stype *z, size_t *n,
          item **items) {
    FILE *file = NULL;
    char buffer[BUFFER_SIZE] = {0};
    int ret = 0;
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        ret = 1;
        goto end;
    }

    if (*instance != NULL) {
        bool instance_found = false;
        while ((fgets(buffer, sizeof(buffer), file) != NULL)) {
            buffer[BUFFER_SIZE - 1] = '\0';
            char *end = strchr(buffer, '\n');
            if (end != NULL) {
                *end = '\0';
            }
            if (strncmp(buffer, *instance, sizeof(buffer)) == 0) {
                instance_found = true;
                break;
            }
            memset(buffer, 0, sizeof(buffer));
        }
        if (!instance_found) {
            fprintf(stderr, "%s: No such a instance\n", *instance);
            ret = 1;
            goto end;
        }
    } else {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            ret = 1;
            fprintf(stderr, "%s: File is empty\n", filename);
            goto end;
        }
        buffer[BUFFER_SIZE - 1] = '\0';
        char *end = strchr(buffer, '\n');
        if (end != NULL) {
            *end = '\0';
        }
        strncpy(instance_name, buffer, strlen(buffer));
        *instance = instance_name;
    }

    size_t idx_item = 0;
    while ((fgets(buffer, sizeof(buffer), file) != NULL)) {
        buffer[BUFFER_SIZE - 1] = '\0';
        char *end = strchr(buffer, '\n');
        if (end != NULL) {
            *end = '\0';
        }
        char line[BUFFER_SIZE] = {0};
        strncpy(line, buffer, sizeof(buffer));

        if (line[0] == 't') {
            continue;
        }
        if (line[0] == '-') {
            break;
        }

        if (line[0] == 'n') {
            strtok(line, " ");
            char *token = strtok(NULL, " ");
            if (token == NULL || my_strtoul(token, n) != 0) {
                fprintf(stderr, "parse error: `%s`\n", buffer);
                ret = 1;
                goto end;
            }
            item *_items = (item *)calloc(*n, sizeof(item));
            if (_items == NULL) {
                perror("calloc");
                ret = 1;
                goto end;
            }
            *items = _items;
        } else if (line[0] == 'c') {
            strtok(line, " ");
            char *token = strtok(NULL, " ");
            if (token == NULL || my_strtol(token, c) != 0) {
                fprintf(stderr, "parse error: `%s`\n", buffer);
                ret = 1;
                goto end;
            }
        } else if (line[0] == 'z') {
            strtok(line, " ");
            char *token = strtok(NULL, " ");
            if (token == NULL || my_strtol(token, z) != 0) {
                fprintf(stderr, "parse error: `%s`\n", buffer);
                ret = 1;
                goto end;
            }
        } else {
            strtok(line, ",");
            char *token_p = strtok(NULL, ",");
            itype p = 0;
            if (token_p == NULL || my_strtol(token_p, &p) != 0) {
                fprintf(stderr, "parse error: `%s`\n", buffer);
                ret = 1;
                goto end;
            }
            char *token_w = strtok(NULL, ",");
            itype w = 0;
            if (token_w == NULL || my_strtol(token_w, &w) != 0) {
                fprintf(stderr, "parse error: `%s`\n", buffer);
                ret = 1;
                goto end;
            }
            item it = {.p = p, .w = w, .x = false};
            (*items)[idx_item++] = it;
        }

        memset(buffer, 0, sizeof(buffer));
    }

end:
    if (ret != 0 && *items != NULL) {
        free(*items);
        *items = NULL;
    }
    if (file != NULL) {
        fclose(file);
        file = NULL;
    }
    return ret;
}

static const struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"instance", required_argument, NULL, 'i'},
    {NULL, 0, NULL, 0},
};

void usage(int argc, char *argv[], bool error) {
    FILE *out = NULL;
    if (error) {
        out = stderr;
    } else {
        out = stdout;
    }

    char *progname = strdup(argv[0]);
    fprintf(out, "Usage: %s [OPTIONS] [FILE]\n", basename(progname));
    fprintf(out, "\n");
    fprintf(out, "Options:\n");
    fprintf(out, "  -i, --instance <instance>    Specify instance name\n");
    fprintf(out, "  -h, --help                   Print Help\n");
    free(progname);
}

int main(int argc, char *argv[]) {
    size_t n = 0;
    item *items = NULL, *orig = NULL;
    stype c = 0, z = 0;
    int ret = 0;
    int opt = 0;
    char *instance = NULL;
    char *filename = NULL;

    while ((opt = getopt_long(argc, argv, "hi:", long_options, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage(argc, argv, false);
            ret = 0;
            goto end;
        case 'i':
            instance = optarg;
            break;
        case '?':
            ret = 1;
            goto end;
        default:
            fprintf(stderr, "Option parse error");
            ret = 1;
            goto end;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Too few arguments\n");
        ret = 1;
        goto end;
    } else if (optind + 1 < argc) {
        fprintf(stderr, "Too many arguments\n");
        ret = 1;
        goto end;
    } else {
        filename = argv[optind];
    }

    struct stat st = {0};
    if (stat(filename, &st) != 0) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        ret = 1;
        goto end;
    }
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "%s: not a regular file\n", filename);
        ret = 1;
        goto end;
    }

    if (parse(filename, &instance, &c, &z, &n, &items) != 0) {
        ret = 1;
        goto end;
    }

    orig = (item *)calloc(n, sizeof(item));
    if (orig == NULL) {
        perror("calloc");
        ret = 1;
        goto end;
    }
    memcpy(orig, items, n * sizeof(item));

    stype obj = combo(items, items + (n - 1), c, 0, 0, true, true);

    for (size_t i = 0; i < n; i++) {
        if (!items[i].x) {
            continue;
        }
        for (size_t j = 0; j < n; j++) {
            if (items[i].p == orig[j].p && items[i].w == orig[j].w &&
                !orig[j].x) {
                orig[j].x = true;
                break;
            }
        }
    }

    printf("Filename : %s\n", basename(filename));
    printf("Instance : %s\n", instance);
    printf("Capacity : %ld\n", c);
    printf("#Items   : %ld\n", n);
    printf("BestKnown: %ld\n", z);
    printf("Objective: %ld\n", obj);
    printf("Solution : [ ");
    for (size_t i = 0; i < n; i++) {
        if (orig[i].x) {
            printf("%ld ", i + 1);
        }
    }
    printf("]\n");

end:
    if (items != NULL) {
        free(items);
        items = NULL;
    }
    if (orig != NULL) {
        free(orig);
        orig = NULL;
    }
    return ret;
}
