#ifndef TREEWALK_FUNC_NATIVE_H
#define TREEWALK_FUNC_NATIVE_H

#include <environment.h>
#include <interpreter_visitor.h>

#include <functional>

namespace plox {
namespace treewalk {
namespace nativefunc {

using Fn =
    std::function<Value(std::shared_ptr<Environment>, InterpreterVisitor &)>;

void addClock(std::shared_ptr<Environment> env);
void addVersion(std::shared_ptr<Environment> env);

} // namespace nativefunc
} // namespace treewalk
} // namespace plox

#endif