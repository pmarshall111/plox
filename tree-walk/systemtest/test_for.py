def test_for(lox_runner):
    # GIVEN
    code = """
    for (var i = 0; i<5; i=i+1)
        print(i);
    ;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["0", "1", "2", "3", "4"]
    assert stderr == ""
