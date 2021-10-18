#include "repl.h"


int main(int argc, char** argv) {

    using namespace Inky::Lisp;

    ReplContext context;
    repl(context);


    return 0;
}
