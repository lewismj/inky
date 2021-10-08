#include <repl.h>

/*
 * Runs the repl for the Lisp environment; allow some
 */
int main(int, char**) {

    inky::repl_context ctx; /* Allow command line params to configure starting context. */
    inky::repl(ctx);

    return 0;
}
