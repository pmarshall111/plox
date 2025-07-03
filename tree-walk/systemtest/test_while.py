def test_while(lox_runner):
    # GIVEN
    code = """
    var a = 15;
    while (a < 42)
        a = a * 2;
    ;
    print(a);
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["60"]
    assert stderr == ""
