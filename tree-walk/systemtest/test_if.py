def test_if(lox_runner):
    # GIVEN
    code = """
    if (15)
        print(15);
    else 
        print(0);
    ;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["15"]
    assert stderr == ""


def test_else(lox_runner):
    # GIVEN
    code = """
    if (15 > 20)
        print(15);
    else 
        print(0);
    ;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["0"]
    assert stderr == ""
