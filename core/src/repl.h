#pragma once

namespace inky {

    /**
     * Defines types & functions for the REPL.
     *  repl_context ; flags that can alter the behaviour of the repl output.
     *  run ; run the repl loop.
     */

    constexpr int FLAG_TRACE = 0x1; /* Set debug flag on, when using the run. */

    /* context for repl, runtime flags etc. */
    struct repl_context {
        int flags = 0;
    };

    /* start the repl-loop. */
    void repl(repl_context& ctx);
}