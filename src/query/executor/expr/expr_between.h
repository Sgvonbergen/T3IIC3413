#pragma once

#include "query/executor/expr/expr.h"
#include "query/parser/logical_plan/logical_plan.h"

class ExprBetween : public Expr {
public:
    ExprBetween(std::unique_ptr<Expr> child, std::unique_ptr<Expr> low, std::unique_ptr<Expr> high) :
        child   (std::move(child)),
        low     (std::move(low)),
        high    (std::move(high)),
        res     ((int64_t)0) { }

    const Value& eval() override {
        auto low_val = low->eval();
        auto high_val = high->eval();
        
        res.value.as_int = static_cast<int64_t>(low_val <= child->eval() && child->eval() <= high_val);
        return res;
    }

    std::ostream& print_to_ostream(std::ostream& os) const override {
        os << *child << " Between " << *low << " AND " << *high;
        return os;
    }

private:
    std::unique_ptr<Expr> child;
    std::unique_ptr<Expr> low;
    std::unique_ptr<Expr> high;

    Value res;
};
