#include <optional>
#include <regex>
#include <string>

#include "parser_t.h"
#include "unicode.h"
#include "value.h"


namespace inky::parser {

    /* tokenize, the input to a value* */
    class tokenizer {
    public:

    };

    either<error,std::vector<value*>> parse(std::string_view input) {
        return error { "not implemented."};
    }
}