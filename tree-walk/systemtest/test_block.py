def test_block(lox_runner):
    # GIVEN
    code = """
    var a = 15;
    {
        var a = 10;
        print(a);
    }
    print(a);
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["10", "15"]
    assert stderr == ""
