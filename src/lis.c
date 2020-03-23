#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "utf8.h"
#include "lstring.h"
#include "stream.h"
#include "runtime.h"
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

void repl(lis_runtime * runtime) {
    lis_stream * stream_stdin = make_lis_stream(1024, LIS_STREAM_IN, LIS_STREAM_TEXT);
    lis_stream * stream_stdout = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    stream_stdin->fin = stdin;
    stream_stdout->fout = stdout;

    char _prompt[] = u8"? ";
    lis_obj * prompt = to_lstring_from_cstr(_prompt, sizeof(_prompt));
    while (true) {
        stream_write_string(stream_stdout, runtime->current_package->data.pkg->name);
        stream_write_string(stream_stdout, prompt);
        stream_flush(stream_stdout);

        lis_obj * obj;
        obj = read(stream_stdin, runtime);

        if (obj == NULL) {
            stream_write_char(stream_stdout, '\n');
            stream_flush(stream_stdout);
            break;
        }

        obj = eval(obj);
        print(stream_stdout, runtime, obj);
        stream_write_char(stream_stdout, '\n');
        stream_flush(stream_stdout);

    }
}

int main(int argc, char** argv) {
    struct Option opt = { "", false };
    if (!parse_option(argc, argv, &opt)) {
        return 1;
    }

    if (opt.help) {
        print_usage();
        return 0;
    }

    lis_runtime * runtime = init_runtime();
    repl(runtime);

    return 0;
}
