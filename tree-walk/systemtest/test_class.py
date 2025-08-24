def test_class(lox_runner):
    # GIVEN
    code = """
    class Test {
        fun printA() {
            print "a";
        }
        fun printB() {
            print "b";
        }
    }

    var tInst = Test();
    tInst.printA();
    tInst.printB();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["a", "b"]
    assert stderr == ""


def test_bound_method_stored_as_var(lox_runner):
    # GIVEN
    code = """
    class Greeter {
        fun getName() {
            return "Greeter";
        }
        fun sayHi() {
            print getName() + " says hi!";
        }
    }

    var greeterHello = Greeter().sayHi;
    greeterHello();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["Greeter says hi!"]
    assert stderr == ""


def test_bound_method_set_on_other_class(lox_runner):
    # GIVEN
    code = """
    class Greeter {
        fun getName() {
            return "Greeter";
        }
        fun sayHi() {
            print getName() + " says hi!";
        }
    }

    class Visitor {
        fun getName() {
            return "Visitor";
        }
        fun sayHi() {
            print getName() + " says -.-"; 
        }
    }

    var greeter = Greeter();
    var visitor = Visitor();
    greeter.sayHi = visitor.sayHi;

    greeter.sayHi();
    visitor.sayHi();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["Visitor says -.-", "Visitor says -.-"]
    assert stderr == ""
