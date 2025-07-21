def test_native_fun(lox_runner):
    # GIVEN
    code = """
    print version()
    ;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["tree-walk"]
    assert stderr == ""


def test_user_fun_args(lox_runner):
    # GIVEN
    code = """
    fun printWithSpace(arg1, arg2) {
        print arg1 + " " + arg2;
    }
    printWithSpace("hello", "world");
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["hello world"]
    assert stderr == ""


def test_fun_fewer_args(lox_runner):
    # GIVEN
    code = """
    fun oneArg(arg1) {
    }
    oneArg();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == []
    assert "accepts 1 args" in stderr.strip()


def test_fun_more_args(lox_runner):
    # GIVEN
    code = """
    fun oneArg(arg1) {
    }
    oneArg("a", "b", "c");
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == []
    assert "accepts 1 args" in stderr.strip()


def test_fun_return(lox_runner):
    # GIVEN
    code = """
    fun nTimes2(n) {
        return n * 2;
    }
    var a = nTimes2(5);
    print a;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["10"]
    assert stderr == ""


def test_fun_empty_return(lox_runner):
    # GIVEN
    code = """
    fun explicitNullReturn() {
        return;
    }
    fun implicitNullReturn() {}
    print explicitNullReturn();
    print implicitNullReturn();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["NULL", "NULL"]
    assert stderr == ""


