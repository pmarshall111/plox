#include <iostream>
#include <map>
#include <strings.h>
#include <memory>
#include <sstream>
#include <utility>

template <typename Derived> // Use CRTP so the base type knows which derived type it is
struct Expr {
    virtual ~Expr() = default;

    template <typename Visitor>
    auto accept(Visitor&& v) {
        return v.visit(*static_cast<Derived *>(this)); // Visit with the derived type.
    }
};

template <typename L, typename R>
struct Binary : public Expr<Binary<L,R>> {
    Binary(std::unique_ptr<Expr<L>> left, std::string_view op, std::unique_ptr<Expr<R>> right) 
        : left(std::move(left)), op(op), right(std::move(right)) {};

    std::unique_ptr<Expr<L>> left;
    std::string_view op;
    std::unique_ptr<Expr<R>> right;
};

struct Literal : public Expr<Literal> {
  Literal(std::string_view _value) : value(_value) {};

  std::string_view value;
};

class PrinterVisitor {
public:
    template <typename L, typename R>
    std::string visit(const Binary<L, R>& bin) {
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

template <typename L, typename R>
auto make_binary(std::unique_ptr<L> left, std::string_view op, std::unique_ptr<R> right) {
    return std::make_unique<Binary<L, R>>(std::move(left), op, std::move(right));
}

int main() {
    auto left = std::make_unique<Literal>("122");
    auto right = std::make_unique<Literal>("2");

    auto bin = make_binary(std::move(left), "+", std::move(right));
    auto litr = std::make_unique<Literal>("hi");
    auto bin2 = make_binary(std::move(bin), "*", std::move(litr));

    // I added the make_binary method to avoid having to nest the template types as below.
    // auto bin2 = std::make_unique<Binary<Binary<Literal, Literal>, Literal>>(std::move(bin), "*", std::move(litr));
    // Alternatively, we could use decltype to get the type of the arguments
    // auto bin2 = std::make_unique<Binary<std::remove_reference_t<decltype(*bin)>, std::remove_reference_t<decltype(*litr)>>>(std::move(bin), "+", std::move(litr));

    PrinterVisitor printerVisitor;
    std::string result = bin2->accept(printerVisitor);

    std::cout << result << std::endl; 
    return 0;
}