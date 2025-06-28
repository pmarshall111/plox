#include <gtest/gtest.h>

#include <environment.h>
#include <errs.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Environment, DefineAndGet) {
  // GIVEN
  Environment env;
  Value initial = 42.0;

  // WHEN
  env.define("x", initial);

  // THEN
  EXPECT_EQ(env.get("x"), initial);
}

TEST(Environment, AssignAndGet) {
  // GIVEN
  Environment env;
  Value initial = 42.0;
  Value updated = "hi";

  // WHEN
  env.define("x", initial);
  env.assign("x", updated);

  // THEN
  EXPECT_EQ(env.get("x"), updated);
}

TEST(Environment, GetFromParentEnv) {
  // GIVEN
  auto parent = std::make_shared<Environment>();
  Value v = 99.0;
  parent->define("x", v);
  Environment child(parent);

  // THEN
  EXPECT_EQ(child.get("x"), v);
}

TEST(Environment, AssignInParentEnv) {
  // GIVEN
  auto parent = std::make_shared<Environment>();
  Value initial = 50.0;
  Value updated = true;
  parent->define("x", initial);
  Environment child(parent);

  // WHEN
  child.assign("x", updated);

  // THEN
  EXPECT_EQ(child.get("x"), updated);
  EXPECT_EQ(parent->get("x"), updated);
}

TEST(Environment, GetUndefinedThrows) {
  Environment env;
  EXPECT_THROW(env.get("missing"), InterpretException);
}

TEST(Environment, AssignUndefinedThrows) {
  Environment env;
  EXPECT_THROW(env.assign("x", {}), InterpretException);
}

} // namespace test
} // namespace treewalk
} // namespace plox
