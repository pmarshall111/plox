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
