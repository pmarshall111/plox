#include <func_native.h>

#include <func.h>

#include <chrono>
#include <functional>

namespace plox {
namespace treewalk {
namespace nativefunc {

void addClock(std::shared_ptr<Environment> env) {
  static std::string s_name = "clock";

  auto clock = [](std::shared_ptr<Environment> env,
                  InterpreterVisitor &interpV) {
    // Use steady_clock bc it's monotonic so won't go backwards when the clocks
    // change unlike system_clock
    auto duration = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration<double, std::milli>(duration).count();
  };

  auto clockFn = std::make_shared<FunctionDescription>(
      s_name, env,
      std::make_shared<Function>(std::vector<std::string_view>{}, clock));
  env->define(s_name, clockFn);
}

void addVersion(std::shared_ptr<Environment> env) {
  static std::string s_name = "version";

  auto version = [](std::shared_ptr<Environment> env,
                    InterpreterVisitor &interpV) { return "tree-walk"; };

  auto versionFn = std::make_shared<FunctionDescription>(
      s_name, env,
      std::make_shared<Function>(std::vector<std::string_view>{}, version));
  env->define(s_name, versionFn);
}

} // namespace nativefunc

} // namespace treewalk
} // namespace plox
