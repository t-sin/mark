#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "core/obj.h"
#include "core/lstring.h"
#include "core/stream.h"
#include "core/list.h"
#include "core/eval.h"
#include "core/print.h"
#include "core/read.h"
#include "core/bootstrap.h"

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

void repl(lis_obj * genv) {
    assert(LIS_TAG_BASE(genv) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(genv) == LIS_TAG_TYPE_ENV);
    assert(genv->data.env->type == LIS_ENV_GLOBAL);

    lis_obj * prompt = LSTR(U"? ");
    lis_stream * stream_stdin = genv->data.env->env.global->stream_stdin->data.stream;
    lis_stream * stream_stdout = genv->data.env->env.global->stream_stdout->data.stream;
    lis_stream * stream_stderr = genv->data.env->env.global->stream_stderr->data.stream;

    while (true) {
        stream_write_string(stream_stdout, genv->data.env->env.global->current_package->data.pkg->name);
        stream_write_string(stream_stdout, prompt);
        stream_flush(stream_stdout);

        lis_obj * obj;
        obj = read(genv, stream_stdin);

        if (LIS_GENV(genv)->error) {
            stream_write_string(stream_stderr, LSTR(U"READER ERROR: "));
            stream_write_string(stream_stderr, LIS_ERR(LIS_GENV(genv)->error)->message);
            stream_write_char(stream_stderr, '\n');
            stream_flush(stream_stderr);
            LIS_GENV(genv)->error = NULL;

            while (!stream_listen_p(stream_stdin)) {
                lis_char ch;
                stream_read_char(stream_stdin, &ch);
            }
            continue;

        } else if (obj == NULL) {
            stream_write_char(stream_stdout, '\n');
            stream_flush(stream_stdout);
            break;
        }

        obj = eval(genv, NULL, obj);

        if (LIS_GENV(genv)->error != NULL) {
            stream_write_string(stream_stderr, LSTR(U"ERROR: "));
            stream_write_string(stream_stderr, LIS_ERR(LIS_GENV(genv)->error)->message);
            stream_write_char(stream_stderr, '\n');
            stream_flush(stream_stderr);
            LIS_GENV(genv)->error = NULL;
        }

        print_mval(genv, obj, stream_stdout);
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

    lis_obj * genv = init_global_env();
    repl(genv);

    return 0;
}
