def test_comments(lox_runner):
    # GIVEN
    code = """
var a = 15; // This is a variable a
var b = 2;
// The above variable is var b
print a + b;
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["17"]
    assert stderr == ""
