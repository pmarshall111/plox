def test_class(lox_runner):
    # GIVEN
    code = """
    class Test {
        printA() {
            print "a";
        }
        printB() {
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


def test_class_takes_scope_of_definition(lox_runner):
    # GIVEN
    code = """
    var global = "global";
    {
        class Greeter {
            printGlobal() {
                print global;
            }
        }

        {
            var global = "scope";
            Greeter().printGlobal();
        }
    }
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["global"]
    assert stderr == ""


def test_class_env_does_not_use_vars_defined_after_class(lox_runner):
    # GIVEN
    code = """
    class Greeter {
        printGlobal() {
            print global;
        }
    }
    var global = "global";
    Greeter().printGlobal();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == []
    assert "Unknown variable" in stderr


def test_bound_method_stored_as_var(lox_runner):
    # GIVEN
    code = """
    class Greeter {
        getName() {
            return "Greeter";
        }
        sayHi() {
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
        getName() {
            return "Greeter";
        }
        sayHi() {
            print getName() + " says hi!";
        }
    }

    class Visitor {
        getName() {
            return "Visitor";
        }
        sayHi() {
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


def test_class_fields(lox_runner):
    # GIVEN
    code = """
    class Greeter {
        printMsg() {
            print this.msg;
        }
    }

    var greeter = Greeter();
    greeter.msg = "Hello there!";
    greeter.printMsg();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["Hello there!"]
    assert stderr == ""


def test_class_init(lox_runner):
    # GIVEN
    code = """
    class Foo {
        init() {
            print "init";
        }
    }

    var foo = Foo();
    var fooInited = foo.init();
    fooInited.init();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["init", "init", "init"]
    assert stderr == ""


def test_class_init_sets_field(lox_runner):
    # GIVEN
    code = """
    class Foo {
        init() {
            fun printMsg() {
                print this.msg;
            }
            this.msg = "hi";
            this.printMsg = printMsg;
        }
    }

    var foo = Foo();
    foo.printMsg();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["hi"]
    assert stderr == ""


def test_class_init_explicit_return(lox_runner):
    # GIVEN
    code = """
    class Foo {
        init() {
            return 1;
        }
    }

    Foo();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == []
    assert "No explicit return" in stderr


def test_class_init_explicit_return_no_val_is_ok(lox_runner):
    # GIVEN
    code = """
    class Foo {
        init() {
            return;
        }
    }

    Foo();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == []
    assert stderr == ""
