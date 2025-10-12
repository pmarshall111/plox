#include <gtest/gtest.h>

#include <environment.h>
#include <errs.h>

namespace plox {
namespace treewalk {
namespace test {

TEST(Environment, DefineAndGet) {
  // GIVEN
  auto envPtr = Environment::create();

  // WHEN
  envPtr->define("x", 42.0);

  // THEN
  EXPECT_EQ(envPtr->get("x"), Value{42.0});
}

TEST(Environment, AssignAndGet) {
  // GIVEN
  auto envPtr = Environment::create();

  // WHEN
  envPtr->define("x", 42.0);
  envPtr->assign("x", "hi");

  // THEN
  EXPECT_EQ(envPtr->get("x"), Value{"hi"});
}

TEST(Environment, UpsertAndGet) {
  // GIVEN
  auto envPtr = Environment::create();

  // WHEN
  envPtr->upsertInScope("x", 42.0);
  envPtr->upsertInScope("x", "hi");

  // THEN
  EXPECT_EQ(envPtr->get("x"), Value{"hi"});
}

TEST(Environment, GetFromParentEnv) {
  // GIVEN
  auto parentPtr = Environment::create();
  parentPtr->define("x", 99.0);
  auto childPtr = Environment::create(parentPtr);

  // THEN
  EXPECT_EQ(childPtr->get("x"), Value{99.0});
}

TEST(Environment, AssignInParentEnv) {
  // GIVEN
  auto parentPtr = Environment::create();
  parentPtr->define("x", 50.0);
  auto childPtr = Environment::create(parentPtr);

  // WHEN
  childPtr->assign("x", true);

  // THEN
  EXPECT_EQ(childPtr->get("x"), Value{true});
  EXPECT_EQ(parentPtr->get("x"), Value{true});
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
