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


def test_redefine(lox_runner):
    # GIVEN
    code = """
    {
        var a = 10;
        fun b() {}
        var a = 12;
    }
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == []
    assert "Cannot redefine" in stderr
