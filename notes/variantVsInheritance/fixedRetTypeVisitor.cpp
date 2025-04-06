#include <iostream>
#include <map>
#include <strings.h>
#include <memory>
#include <sstream>
#include <utility>

struct Binary;
struct Literal;

struct Visitor {
    virtual std::string visit(const Binary&) = 0;
    virtual std::string visit(const Literal&) = 0;
};

struct Expr {
    virtual ~Expr() = default;

    virtual std::string accept(Visitor& v) = 0;
};

struct Binary : public Expr {
    Binary(std::unique_ptr<Expr> left, std::string_view op, std::unique_ptr<Expr> right) 
        : left(std::move(left)), op(op), right(std::move(right)) {};

    std::string accept(Visitor& v) override {
        return v.visit(*this);
    }

    std::unique_ptr<Expr> left;
    std::string_view op;
    std::unique_ptr<Expr> right;
};

struct Literal : public Expr {
    Literal(std::string_view _value) : value(_value) {};

    std::string accept(Visitor& v) override {
        return v.visit(*this);
    }

    std::string_view value;
};

class PrinterVisitor: public Visitor {
public:
    std::string visit(const Binary& bin) {
        std::ostringstream ss;
        ss << bin.left->accept(*this) << " " << bin.op 
           << " " << bin.right->accept(*this);
        return addParens(ss.str());
    }

    std::string visit(const Literal& ltrl) {
        return std::string(ltrl.value);
    }

private:
    std::string addParens(const std::string& expr) {
        return "(" + expr + ")";
    }
};

int main() {
    auto left = std::make_unique<Literal>("122");
    auto right = std::make_unique<Literal>("2");

    auto bin = std::make_unique<Binary>(std::move(left), "+", std::move(right));
    auto litr = std::make_unique<Literal>("hi");
    auto bin2 = std::make_unique<Binary>(std::move(bin), "*", std::move(litr));

    PrinterVisitor printerVisitor;
    std::string result = bin2->accept(printerVisitor);

    std::cout << result << std::endl; 
    return 0;
}