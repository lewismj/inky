#pragma once

namespace Inky::Lisp {

    /* Define any flags for REPL commands. */
    constexpr int FLAG_DEBUG= 0x1;

    /* Context holds the stat of the flags, etc. */
    struct ReplContext { int flags = 0; };

    void repl(ReplContext & ctx); /* Run the REPL. */

}