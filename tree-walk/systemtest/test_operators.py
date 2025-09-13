def test_operators(lox_runner):
    # GIVEN
    code = """
    print "one " + 1;
    print "two " + "two";
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["one 1.000000", "two two"]
    assert stderr == ""
