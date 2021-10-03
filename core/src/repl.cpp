#include <iostream>
#include <string>
#include <fmt/color.h>

#include "repl.h"
#include "parser.h"

namespace inky::repl {

    void print_eval(repl_context &ctx, std::string_view input) {
    }

    void run_repl_command(repl_context &ctx, std::string_view input) {
        auto printf = [&ctx](std::string_view input, int flag) {
            bool enabled = (ctx.flags & flag);
            auto clr = enabled ? fg(fmt::terminal_color::green) : fg(fmt::terminal_color::red);
            auto msg = enabled ? "enabled" : "disabled";
            fmt::print(clr | (fmt::emphasis::bold), "{}::{}\n", input, msg);
        };

        if (input == ":t") {
            ctx.flags ^= FLAG_TRACE;
            printf("debug trace", FLAG_TRACE);
        }
    }

    void run(repl_context &ctx) {
        //ctx.flags |= FLAG_TRACE;
        ctx.flags = 0;

        while (true) {
            fmt::print("λ> ");
            std::string input;
            std::getline(std::cin, input);

            if (std::cin.eof() || input == ":q") break;
            else if (input[0] == ':') run_repl_command(ctx, input);
            else if (input[0] == ';') { /* ignore, first character is comment, so skip entire line. */ }
            else print_eval(ctx, input);
        }
    }
}
