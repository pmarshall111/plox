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
