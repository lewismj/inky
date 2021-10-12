#include <repl.h>

int main(int, char**) { /* Run the Lisp repl. */

    inky::repl_context ctx; /* Allow command line params to configure starting context. */
    inky::repl(ctx);

    return 0;
}
