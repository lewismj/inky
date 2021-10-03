#pragma once

namespace inky::repl {

    constexpr int FLAG_TRACE = 0x1; /* Set debug flag on, when using the run. */

    /* context for repl, runtime flags etc. */
    struct repl_context {
        int flags = 0;
    };

    /* start the repl-loop. */
    void run(repl_context &ctx);
}