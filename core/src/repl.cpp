#include <iostream>
#include <string>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "value.h"
#include "environment.h"
#include "builtin.h"
#include "parser.h"
#include "eval.h"
#include "repl.h"
#include "types.h"

namespace inky {

    class repl_impl {
    public:

        explicit repl_impl(repl_context context) : ctx(context), env(new environment()) {}
        ~repl_impl() = default;

        void parse_eval(std::string_view input) {
            auto v = inky::parse(input);
            if ( v.is_right() ) {
                either<error,value_ptr> result = eval(env,v.right_value());
                auto clr = result ? fg(fmt::terminal_color::green) | (fmt::emphasis::bold)
                        : fg(fmt::terminal_color::red) | (fmt::emphasis::bold);

                if (result) fmt::print(clr,"{}\n",result.right_value());
                else fmt::print(clr,"{}\n",result.left_value().message);
            } else {
                error e = v.left_value();
                fmt::print( fg(fmt::terminal_color::green) | (fmt::emphasis::italic), "{}\n",e.message);
                fmt::print( fg(fmt::terminal_color::green) | (fmt::emphasis::italic), "{}\n",input);

                std::string underline;
                if(e.loc.length >= 1) for(size_t i = 0; i < e.loc.length; i++) underline += "\u203e";
                std::string s(e.loc.begin,' ');
                fmt::print(fg(fmt::terminal_color::red) | (fmt::emphasis::bold), "{}{}\n",s,underline);
            }
        }

        void run_repl_command(std::string_view input) {
            auto printf = [&](std::string_view input, int flag) {
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

        void run() {
            repl_context ctx;
            ctx.flags = 0;

            /* Put builtin functions into the environment & load Lisp prelude. */
            inky::builtin::add_builtin_functions(env);

            /* TODO: Load prelude. */

            while (true) {
                fmt::print("λ> ");
                std::string input;
                std::getline(std::cin, input);

                if (std::cin.eof() || input == ":q") break;
                else if (input[0] == ':') run_repl_command(input);
                else if (input[0] == ';') { /* ignore, first character is comment, so skip entire line. */ }
                else parse_eval(input);
            }
        }

    private:
        environment_ptr env;
        repl_context ctx;
    };


    void repl(repl_context& ctx) {
        repl_impl r(ctx);
        r.run();
    }

}
