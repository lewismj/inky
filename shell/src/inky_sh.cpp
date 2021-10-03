#include <repl.h>

/*
 * Runs the repl for the Lisp environment; allow some
 */
int main(int, char**) {

    inky::repl::repl_context ctx;
    inky::repl::run(ctx);

    return 0;
}
