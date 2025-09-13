#include <interpreter.h>

#include <ast_printer.h>
#include <class.h>
#include <func.h>
#include <value_printer.h>

#include <charconv>
#include <iostream>

namespace plox {
namespace treewalk {

void interpret(std::vector<stmt::Stmt> &stmts,
               std::shared_ptr<Environment> &env,
               std::vector<InterpretException> &errs) {
  try {
    InterpreterVisitor v{env};
    for (auto &s : stmts) {
      std::visit(v, s);
    }
  } catch (const InterpretException &e) {
    errs.push_back(e);
  }
}

using namespace ast;
using namespace stmt;

namespace {
static ValuePrinter s_valuePrinter;

struct AdditionVisitor {
  Value operator()(double l, double r);
  Value operator()(std::string &l, std::string &r);
  Value operator()(auto &&l, auto &&r);
} s_adder;

struct SubtractionVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} s_subtractor;

struct MultiplyVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} s_multiplier;

struct DivideVisitor {
  double operator()(double l, double r);
  double operator()(auto &&l, auto &&r);
} s_divider;

struct TruthyVisitor {
  bool operator()(std::monostate);
  bool operator()(bool b);
  bool operator()(double b);
  bool operator()(auto &&);
} s_truther;

struct ReturnEx : public std::runtime_error {
  ReturnEx(Value v) : d_val(v), std::runtime_error("return"){};
  Value d_val;
};
} // namespace

InterpreterVisitor::InterpreterVisitor(std::shared_ptr<Environment> &env)
    : d_env(env) {}

void InterpreterVisitor::operator()(const Block &blk) {
  // Create new scope and restore it after this func
  std::shared_ptr<Environment> newEnv = Environment::create(d_env);
  environmentutils::ScopedSwap swapGuard(d_env, newEnv);

  // Run statements within block now new env is installed
  for (auto &stmt : blk.stmts) {
    std::visit(*this, *stmt);
  }
}

void InterpreterVisitor::operator()(const Class &cls) {
  // Retrieve the super class
  std::shared_ptr<ClassDefinition> super;
  if (cls.super) {
    Value v = d_env->get(std::string(cls.super.value()));
    if (!std::holds_alternative<ClsDefShrdPtr>(v)) {
      throw InterpretException("Super class for " + std::string(cls.name) +
                               "must be a class");
    }
    super = std::get<ClsDefShrdPtr>(v);
  }

  // Create a new environment for the class where the methods will be defined.
  // Note, a class keeps the environment from the point of definition, so we
  // capture the current environment here.
  std::shared_ptr<Environment> clsEnv = Environment::create(d_env);

  // Create the class factory which will be used to create instances.
  d_env->define(std::string(cls.name),
                std::make_shared<ClassDefinition>(cls.name, clsEnv, super));

  // Set the interpreter environment to be the class environment and add the
  // methods
  {
    environmentutils::ScopedSwap swapGuard(d_env, clsEnv);
    for (auto &m : cls.methods) {
      std::visit(*this, *m);
    }
  }

  // Now extend the current environment so variables defined after this don't
  // get defined in the Environment captured by the Class
  d_env = Environment::extend(d_env);
}

void InterpreterVisitor::operator()(const For &forStmt) {
  if (forStmt.initialiser) {
    std::visit(*this, *forStmt.initialiser);
  }

  auto condition = [&]() {
    if (forStmt.condition) {
      return std::visit(s_truther, std::visit(*this, *forStmt.condition));
    }
    // It's possible to have no condition - in that case the loop should run
    // forever
    return true;
  };

  while (condition()) {
    std::visit(*this, *forStmt.body);
    if (forStmt.incrementer) {
      std::visit(*this, *forStmt.incrementer);
    }
  }
}

void InterpreterVisitor::operator()(Fun &funStmt) {
  // Create a function object and store it in the current env
  auto f = std::make_shared<FunctionDescription>(
      funStmt.name, d_env,
      std::make_shared<Function>(std::move(funStmt.params),
                                 std::move(funStmt.stmts)));
  d_env->define(std::string(funStmt.name), f);

  if (!funStmt.isMethod) {
    // Extend scope so this function can have an Environment with only the
    // currently defined vars for the scope. Note, methods should know about
    // everything within the class so we don't extend in this case.
    std::shared_ptr<Environment> scopeExt = Environment::extend(d_env);
    std::swap(d_env, scopeExt);
  }
  if (funStmt.isMethod && funStmt.name == "init") {
    f->setIsInitialiser(true);
  }
}

void InterpreterVisitor::operator()(const Expression &expr) {
  std::visit(*this, *expr.expr);
}

void InterpreterVisitor::operator()(const If &ifStmt) {
  Value evaluatedCondition = std::visit(*this, *ifStmt.condition);
  bool isTruthy = std::visit(s_truther, evaluatedCondition);
  if (isTruthy) {
    std::visit(*this, *ifStmt.ifBranch);
  } else if (ifStmt.elseBranch) {
    std::visit(*this, *ifStmt.elseBranch);
  }
}

void InterpreterVisitor::operator()(const Print &print) {
  // Calculate expression
  Value v = std::visit(*this, *print.expr);
  // Print
  std::cout << std::visit(s_valuePrinter, v) << std::endl;
}

void InterpreterVisitor::operator()(const Return &ret) {
  Value v = {};
  if (ret.expr) {
    v = std::visit(*this, *ret.expr);
  }
  throw ReturnEx{v};
}

void InterpreterVisitor::operator()(const VarDecl &varDecl) {
  auto name = std::string(varDecl.name);
  Value val = {};
  if (varDecl.expr) {
    val = std::visit(*this, *varDecl.expr);
  }
  d_env->define(name, val);
}

void InterpreterVisitor::operator()(const While &whileStmt) {
  while (std::visit(s_truther, std::visit(*this, *whileStmt.condition))) {
    std::visit(*this, *whileStmt.body);
  }
}

Value InterpreterVisitor::operator()(const Assign &assign) {
  auto name = std::string(assign.name);
  Value val = std::visit(*this, *assign.value);
  d_env->assign(name, val);
  return val;
}

Value InterpreterVisitor::operator()(const Binary &bnry) {
  Value lhs = std::visit(*this, *bnry.left);
  Value rhs = std::visit(*this, *bnry.right);

  switch (bnry.op.type) {
  case TokenType::PLUS:
    return std::visit(s_adder, lhs, rhs);
  case TokenType::MINUS:
    return std::visit(s_subtractor, lhs, rhs);
  case TokenType::STAR:
    return std::visit(s_multiplier, lhs, rhs);
  case TokenType::SLASH:
    return std::visit(s_divider, lhs, rhs);
  case TokenType::EQUAL_EQUAL:
    return lhs == rhs;
  case TokenType::BANG_EQUAL:
    return lhs != rhs;
  case TokenType::GREATER:
    return lhs > rhs;
  case TokenType::GREATER_EQUAL:
    return lhs >= rhs;
  case TokenType::LESS:
    return lhs < rhs;
  case TokenType::LESS_EQUAL:
    return lhs <= rhs;
  default:
    throw InterpretException("Unable to interpret binary op: " +
                             tokenutils::tokenTypeToStr(bnry.op.type));
  }
}

Value InterpreterVisitor::invoke(const FnDescShrdPtr &fnDescSPtr,
                                 const Call &call) {
  if (!fnDescSPtr) {
    throw InterpretException(
        "Internal error! Function closure pointer is null!");
  }
  auto fnSPtr = fnDescSPtr->getFunction();
  if (!fnSPtr) {
    throw InterpretException("Internal error! Function pointer is null!");
  }

  // Check if the number of args matches the callee args
  if (call.args.size() != fnSPtr->getArity()) {
    std::ostringstream ss;
    ss << "Tried to call " << fnDescSPtr->getName() << " with "
       << call.args.size() << " args when function accepts "
       << fnSPtr->getArity() << " args.";
    throw InterpretException(ss.str());
  }

  // Create a new environment for the func to execute in
  std::shared_ptr<Environment> fEnv =
      Environment::create(fnDescSPtr->getClosure());

  // Set args in new environment
  const std::vector<std::string_view> &fArgNames = fnSPtr->getArgNames();
  for (int i = 0; i < call.args.size(); i++) {
    Value v = std::visit(*this, *call.args[i]);
    fEnv->define(std::string(fArgNames[i]), v);
  }

  // Update environment to be the environment of the function, and swap back on
  // destruction
  environmentutils::ScopedSwap swapGuard(d_env, fEnv);

  // Special behaviour for initialisers - always return "this"
  if (fnDescSPtr->isInitialiser()) {
    Value _this = fnDescSPtr->getClosure()->get("this");
    try {
      fnSPtr->execute(d_env, *this);
    } catch (ReturnEx &ex) {
      if (!std::holds_alternative<std::monostate>(ex.d_val)) {
        throw InterpretException(
            "No explicit return allowed from a class initialiser");
      }
    }
    return _this;
  }

  try {
    // Pass execution to function. If the user has written a return statement it
    // will throw and be caught below
    return fnSPtr->execute(d_env, *this);
  } catch (ReturnEx &ex) {
    return ex.d_val;
  }
}

Value InterpreterVisitor::invoke(const ClsDefShrdPtr &clsDefSPtr,
                                 const Call &call) {
  if (!clsDefSPtr) {
    throw InterpretException("Internal error! Class factory pointer is null!");
  }

  // Create class instance for every class in the heirarchy. This allows for
  // each level of the heirarchy to have its own environment containing the
  // methods for the level.
  auto currDef = clsDefSPtr;
  std::shared_ptr<ClassInstance> currClass;
  std::shared_ptr<ClassInstance> childOfCurrent;
  std::shared_ptr<ClassInstance> leafClass;
  do {
    // Copy the functions from the Definition into a new environment.
    auto currEnv =
        std::shared_ptr<Environment>(new Environment(*currDef->getClosure()));
    for (const auto &[k, v] : *currEnv) {
      auto fnDefCopy =
          std::make_shared<FunctionDescription>(*std::get<FnDescShrdPtr>(v));
      // Bind the current environment to the function so the member function can
      // be stored in a variable outside the class.
      fnDefCopy->getClosure() = currEnv;
      currEnv->assign(k, fnDefCopy);
    }

    // Create ClassInstance object for the current class in the inheritance
    // tree.
    currClass = std::make_shared<ClassInstance>(currDef->getName(), currEnv);
    if (!leafClass) {
      leafClass = currClass;
    }
    // Link the heirarchy together through 'this' and 'super'
    currEnv->define("this", leafClass);
    if (childOfCurrent) {
      childOfCurrent->getClosure()->define("super", currClass);
    }

    // Prepare next iteration
    childOfCurrent = currClass;
    currDef = currDef->getSuper();
  } while (currDef);

  if (leafClass->getClosure()->isVarInScope("init")) {
    invoke(std::get<FnDescShrdPtr>(leafClass->getClosure()->get("init")), call);
  }

  return leafClass;
}

Value InterpreterVisitor::operator()(const Call &call) {
  // Evaluate the callee. Normally this would just be a function name, but
  // in chains we may need to evaluate a preceeding function i.e. fn(1)(2);
  Value callee = std::visit(*this, *call.callee);

  if (std::holds_alternative<FnDescShrdPtr>(callee)) {
    return invoke(std::get<FnDescShrdPtr>(callee), call);
  } else if (std::holds_alternative<ClsDefShrdPtr>(callee)) {
    return invoke(std::get<ClsDefShrdPtr>(callee), call);
  } else {
    throw InterpretException("Tried to call non callable object " +
                             std::visit(s_valuePrinter, callee));
  }
}

Value InterpreterVisitor::operator()(const Get &get) {
  // Retrieve the object we're getting from. Normally this would just be a class
  // instance, but we may need to evaluate a function call before we can access
  // the object i.e. getCreationFactory().create()
  Value obj = std::visit(*this, *get.object);
  if (!std::holds_alternative<ClsInstShrdPtr>(obj)) {
    throw InterpretException("Tried to get a property on non class instance " +
                             std::visit(s_valuePrinter, obj));
  }

  auto clsInst = std::get<ClsInstShrdPtr>(obj);
  return clsInst->getClosure()->get(std::string(get.property));
}

Value InterpreterVisitor::operator()(const Grouping &grp) {
  return std::visit(*this, *grp.expr);
}

namespace {

double getNum(const Literal &ltrl) {
  double val;
  auto start = ltrl.value.data();
  auto end = ltrl.value.data() + ltrl.value.size();
  auto [parseEnd, ec] = std::from_chars(start, end, val);
  if (ec == std::errc::result_out_of_range) {
    throw InterpretException("Number too large: " + std::string(ltrl.value));
  }
  if (parseEnd != end) {
    throw InterpretException("Unable to read number: " +
                             std::string(ltrl.value));
  }
  return val;
}
} // namespace

Value InterpreterVisitor::operator()(const Literal &ltrl) {
  switch (ltrl.type) {
  case TokenType::STRING:
    return std::string(ltrl.value);
  case TokenType::NUMBER: {
    return getNum(ltrl);
  }
  case TokenType::TRUE:
    return true;
  case TokenType::FALSE:
    return false;
  case TokenType::NUL:
    return {};
  default:
    throw InterpretException("Unable to interpret type: " +
                             tokenutils::tokenTypeToStr(ltrl.type));
  }
}

Value InterpreterVisitor::operator()(const Set &set) {
  Value obj = std::visit(*this, *set.object);
  if (!std::holds_alternative<ClsInstShrdPtr>(obj)) {
    throw InterpretException("Tried to set a property on non class instance " +
                             std::visit(s_valuePrinter, obj));
  }

  Value val = std::visit(*this, *set.value);
  if (std::holds_alternative<FnDescShrdPtr>(val)) {
    FnDescShrdPtr fnCopy =
        std::make_shared<FunctionDescription>(*std::get<FnDescShrdPtr>(val));
    fnCopy->setName(set.property);
    fnCopy->setIsInitialiser(set.property == "init");
    val = fnCopy;
  }
  std::get<ClsInstShrdPtr>(obj)->getClosure()->upsertInScope(
      std::string(set.property), val);
  return {};
}

Value InterpreterVisitor::operator()(const Unary &unry) {
  Value right = std::visit(*this, *unry.right);
  switch (unry.op.type) {
  case TokenType::MINUS: {
    Value zero = 0.0;
    return std::visit(s_subtractor, zero, right);
  }
  case TokenType::BANG:
    return !std::visit(s_truther, right);
  default:
    throw InterpretException("Unable to interpret unary op: " +
                             tokenutils::tokenTypeToStr(unry.op.type));
  }
}

Value InterpreterVisitor::operator()(const Variable &var) {
  return d_env->get(std::string(var.name));
}

Value AdditionVisitor::operator()(double l, double r) { return l + r; }

Value AdditionVisitor::operator()(std::string &l, std::string &r) {
  return l + r;
}

Value AdditionVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to add types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
}

double SubtractionVisitor::operator()(double l, double r) { return l - r; }

double SubtractionVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to subtract types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
}

double MultiplyVisitor::operator()(double l, double r) { return l * r; }

double MultiplyVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to multiply types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
}

double DivideVisitor::operator()(double l, double r) { return l / r; }

double DivideVisitor::operator()(auto &&l, auto &&r) {
  throw InterpretException(
      "Unable to divide types: " + std::string(typeid(l).name()) + " and " +
      std::string(typeid(r).name()));
};

bool TruthyVisitor::operator()(std::monostate) { return false; }

bool TruthyVisitor::operator()(bool b) { return b; }

bool TruthyVisitor::operator()(double d) { return static_cast<bool>(d); }

bool TruthyVisitor::operator()(auto &&) { return true; }

} // namespace treewalk
} // namespace plox