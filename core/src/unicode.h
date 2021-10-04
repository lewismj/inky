#pragma once

#include <utf8proc.h>


/* TBD: Not currently used. */

namespace inky::unicode
{

    size_t is_category(std::string_view input, const std::initializer_list<int>& categories) {
        utf8proc_int32_t cp = { };
        auto sz = utf8proc_iterate((const uint8_t*) input.data(), input.size(), &cp);
        if(cp == -1) return 0;

        auto cat = utf8proc_category(cp);
        for(auto c : categories) if(cat == c) return sz;

        return 0;
    }

    size_t get_codepoint_length(std::string_view input) {
        utf8proc_int32_t cp = { };
        auto sz = utf8proc_iterate((const uint8_t*) input.data(), input.size(), &cp);
        if(cp == -1) return 1;

        return sz;
    }

    size_t is_letter(std::string_view input) {
        return is_category(input, {
                UTF8PROC_CATEGORY_LU, UTF8PROC_CATEGORY_LL, UTF8PROC_CATEGORY_LT,
                UTF8PROC_CATEGORY_LM, UTF8PROC_CATEGORY_LO
        });
    }

    size_t is_digit(std::string_view str) {
        return is_category(str, { UTF8PROC_CATEGORY_ND });
    }

}
