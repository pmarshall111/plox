#include <iostream>
#include <map>
#include <strings.h>
#include <memory>
#include <sstream>
#include <utility>
#include <variant>

struct Binary;
struct Literal;

using Expr = std::variant<Binary,Literal>;

struct Binary {
    std::unique_ptr<Expr> left;
    std::string_view op;
    std::unique_ptr<Expr> right;
};

struct Literal {
    std::string_view value;
};


class PrinterVisitor {
public:
    std::string operator()(const Binary& bin) {
        std::ostringstream ss;
        ss << std::visit(*this, *bin.left) << " " << bin.op 
           << " " << std::visit(*this, *bin.right);
        return addParens(ss.str());
    }

    std::string operator()(const Literal& ltrl) {
        return std::string(ltrl.value);
    }

private:
    std::string addParens(const std::string& expr) {
        return "(" + expr + ")";
    }
};

int main() {
    auto left = std::make_unique<Expr>(Literal{"122"});
    auto right = std::make_unique<Expr>(Literal{"2"});

    auto bin = std::make_unique<Expr>(Binary{std::move(left), "+", std::move(right)});
    auto litr = std::make_unique<Expr>(Literal{"hi"});
    auto bin2 = std::make_unique<Expr>(Binary{std::move(bin), "*", std::move(litr)});

    PrinterVisitor printerVisitor;
    std::string result = std::visit(printerVisitor, *bin2);

    std::cout << result << std::endl; 
    return 0;
}
