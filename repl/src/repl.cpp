#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "builtin.h"
#include "either.h"
#include "environment.h"
#include "eval.h"
#include "parser.h"
#include "repl.h"


namespace Inky::Lisp {

    class Repl {
    public:
        explicit Repl(ReplContext& context) : ctx(context) , env(new Environment()) {}
        ~Repl() = default;


        void parseAndEvalInput(std::string_view input) {
            auto v = parse(input);
            if ( v) {
                Either<Error,ValuePtr> result = eval(env,v.right());
                auto clr = result ? fg(fmt::terminal_color::green) | (fmt::emphasis::bold)
                                  : fg(fmt::terminal_color::red) | (fmt::emphasis::bold);

                if (result) fmt::print(clr,"{}\n",result.right());
                else fmt::print(clr,"{}\n",result.left().message);
            } else {
                Error e = v.left();
                fmt::print( fg(fmt::terminal_color::green) | (fmt::emphasis::italic), "{}\n",e.message);
                fmt::print( fg(fmt::terminal_color::green) | (fmt::emphasis::italic), "{}\n",input);

                std::string underline;
                if(e.location.length >= 1) for(size_t i = 0; i < e.location.length; i++) underline += "\u203e";
                std::string s(e.location.begin,' ');
                fmt::print(fg(fmt::terminal_color::red) | (fmt::emphasis::bold), "{}{}\n",s,underline);
            }
        }

        void runReplCommand(std::string_view input) {
            auto printf = [&](std::string_view input, int flag) {
                bool enabled = (ctx.flags & flag);
                auto clr = enabled ? fg(fmt::terminal_color::green) : fg(fmt::terminal_color::red);
                auto msg = enabled ? "enabled" : "disabled";
                fmt::print(clr | (fmt::emphasis::bold), "{}::{}\n", input, msg);
            };

            if (input == ":t") {
                ctx.flags ^= FLAG_DEBUG;
                printf("debug trace", FLAG_DEBUG);
            }
        }

        void run() {

            addBuiltinFunctions(env);

            /* TODO; load the prelude. */

            while (true) {
                fmt::print("λ> ");
                std::string input;
                std::getline(std::cin, input);

                if (std::cin.eof() || input == ":q") break;
                else if (input[0] == ':') runReplCommand(input);
                else if (input[0] == ';') { /* ignore, first character is comment, so skip entire line. */ }
                else parseAndEvalInput(input);
            }
        }

    private:
        ReplContext     ctx; /* Repl context, flags...  */
        EnvironmentPtr  env; /* Global environment.     */
    };


    void repl(ReplContext & ctx) {
        Repl r(ctx);
        r.run();
    }


}