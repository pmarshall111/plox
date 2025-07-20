#include <func_native.h>

#include <func.h>

#include <chrono>
#include <functional>

namespace plox {
namespace treewalk {
namespace nativefunc {

namespace {
Value clock(std::shared_ptr<Environment> env, InterpreterVisitor &interpV) {
  // Use steady_clock bc it's monotonic so won't go backwards when the clocks
  // change unlike system_clock
  auto duration = std::chrono::steady_clock::now().time_since_epoch();
  return std::chrono::duration<double, std::milli>(duration).count();
}
} // namespace

void addClock(std::shared_ptr<Environment> env) {
  static std::string_view s_name = "clock";
  auto clockFn = std::make_shared<Function>(
      s_name, std::vector<std::string_view>{}, env, clock);
  env->define("clock", clockFn);
}

} // namespace nativefunc

} // namespace treewalk
} // namespace plox
