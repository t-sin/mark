#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "obj.h"
#include "utf8.h"
#include "stream.h"
#include "eval.h"
#include "print.h"
#include "read.h"

void print_usage() {
    printf("usage: lis LISPATH\n");
    printf("\n");
    printf("OPTIONS\n");
    printf("    -h --help       Print this help.\n");
    printf("\n");
}

struct Option {
    char* lispath;
    bool help;
};

#define optcmp(sname, lname) \
(short_opt && strcmp(argv[i]+1, sname) == 0) || (long_opt && strcmp(argv[i]+2, lname) == 0)

bool parse_option(int argc, char** argv, struct Option* opt) {
    bool lispath_filled = false;

    for (int i=1; i<argc; i++) {
        bool short_opt = false;
        bool long_opt = false;

        if (argv[i][0] == '-') {
            if (strlen(argv[i]) >= 2 && argv[i][1] == '-') {
                long_opt = true;
            } else {
                short_opt = true;
            }

            if (optcmp("h", "help")) {
                printf("aaaa\n");
                opt->help = true;
            }

        } else {
            if (!lispath_filled) {
                lispath_filled = true;
                opt->lispath = argv[i];
            } else {
                printf("LISPATH specified twice?\n");
                return false;
            }
        }
    }

    return true;
}

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    struct Option opt = { "", false };
    if (!parse_option(argc, argv, &opt)) {
        return 1;
    }

    if (opt.help) {
        print_usage();
        return 0;
    }

    lis_stream * stream_stdin = make_lis_stream(1024, LIS_STREAM_IN, LIS_STREAM_TEXT);
    lis_stream * stream_read = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    stream_stdin->fin = stdin;

    while (true) {
        fprintf(stdout, "? ");
        fflush(stdout);

        lis_char ch;
        fflush(stream_stdin->fin);

        lis_obj * obj = read(stream_stdin);
        print(obj);
        fprintf(stdout, "\n");

        if (obj == NULL) {
            break;
        }
    }

    return 0;
}
