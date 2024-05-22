#pragma once

#include "query/executor/expr/expr.h"
#include "query/parser/logical_plan/logical_plan.h"

class ExprLike : public Expr {
public:
    ExprLike(std::unique_ptr<Expr> child, std::string&& _pattern) :
        child   (std::move(child)),
        pattern (std::move(_pattern)),
        res     ((int64_t)0) { }

    const Value& eval() override {
        bool matches = false;
        bool wildcard = pattern[0] == '%';
        char* child_text = child->eval().value.as_str;
        uint32_t child_pos = 0;
        uint32_t patt_pos = wildcard ? 1 : 0;
        while (true) {
            // Pass if both strings are on end.
            if (child_text[child_pos] == '\0' && pattern[patt_pos] == '\0') {
                matches = true;
                break;
            }
            // Fail if only one of the strings is on end. (if on wildcard mode, keep moving on child)
            if (pattern[patt_pos] == '\0' || child_text[child_pos] == '\0') {
                matches = false;
                break;
            }
            // Pattern matching.
            if (child_text[child_pos] == pattern[patt_pos] || pattern[patt_pos] == '_') {
                child_pos++;
                patt_pos++;
                wildcard = false;
            // Pass, Found % on pattern end.
            } else if (pattern[patt_pos] == '%') {
                matches = true;
                break;
            // Started with %, keep moving on child until match
            } else if (wildcard) {
                child_pos++;
                continue;
            // Fail. Child and Pattern didn't match
            } else {
                matches = false;
                break;
            }
        }

        res.value.as_int = matches ? static_cast<int64_t>(1) : static_cast<int64_t>(0);
        return res;
    }

    std::ostream& print_to_ostream(std::ostream& os) const override {
        os << *child << " LIKE \"" << pattern << '"';
        return os;
    }

private:
    std::unique_ptr<Expr> child;

    std::string pattern;

    Value res;
};
