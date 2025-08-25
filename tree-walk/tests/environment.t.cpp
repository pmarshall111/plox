#include <gtest/gtest.h>

#include <environment.h>
#include <errs.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Environment, DefineAndGet) {
  // GIVEN
  auto envPtr = Environment::create();
  Value initial = 42.0;

  // WHEN
  envPtr->define("x", initial);

  // THEN
  EXPECT_EQ(envPtr->get("x"), initial);
}

TEST(Environment, AssignAndGet) {
  // GIVEN
  auto envPtr = Environment::create();
  Value initial = 42.0;
  Value updated = "hi";

  // WHEN
  envPtr->define("x", initial);
  envPtr->assign("x", updated);

  // THEN
  EXPECT_EQ(envPtr->get("x"), updated);
}

TEST(Environment, UpsertAndGet) {
  // GIVEN
  auto envPtr = Environment::create();
  Value initial = 42.0;
  Value updated = "hi";

  // WHEN
  envPtr->upsert("x", initial);
  envPtr->upsert("x", updated);

  // THEN
  EXPECT_EQ(envPtr->get("x"), updated);
}

TEST(Environment, GetFromParentEnv) {
  // GIVEN
  auto parentPtr = Environment::create();
  Value v = 99.0;
  parentPtr->define("x", v);
  auto childPtr = Environment::create(parentPtr);

  // THEN
  EXPECT_EQ(childPtr->get("x"), v);
}

TEST(Environment, AssignInParentEnv) {
  // GIVEN
  auto parentPtr = Environment::create();
  Value initial = 50.0;
  Value updated = true;
  parentPtr->define("x", initial);
  auto childPtr = Environment::create(parentPtr);

  // WHEN
  childPtr->assign("x", updated);

  // THEN
  EXPECT_EQ(childPtr->get("x"), updated);
  EXPECT_EQ(parentPtr->get("x"), updated);
}

TEST(Environment, GetUndefinedThrows) {
  auto envPtr = Environment::create();
  EXPECT_THROW(envPtr->get("missing"), InterpretException);
}

TEST(Environment, AssignUndefinedThrows) {
  auto envPtr = Environment::create();
  EXPECT_THROW(envPtr->assign("x", {}), InterpretException);
}

TEST(Environment, multipleEnvForScope) {
  // GIVEN
  auto globalPtr = Environment::create();
  globalPtr->define("x", 12.0);

  auto scopePtr = Environment::create(globalPtr);
  scopePtr->define("y", 50.0);

  auto scopePtr2 = Environment::extend(scopePtr);
  scopePtr2->define("z", 15.0);

  // WHEN
  auto tailOfScope = Environment::extend(scopePtr2);

  // THEN
  EXPECT_THROW(scopePtr->define("newVar", "_"), InterpretException);
  EXPECT_NO_THROW(tailOfScope->define("x", "abc"));
  EXPECT_THROW(tailOfScope->define("y", "abc"), InterpretException);
}

} // namespace test
} // namespace treewalk
} // namespace plox
