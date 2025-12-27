def test_nil(lox_runner):
    # GIVEN
    code = """
    var a = nil;
    print a;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["NULL"]
    assert stderr == ""
