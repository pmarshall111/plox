def test_block(lox_runner):
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
