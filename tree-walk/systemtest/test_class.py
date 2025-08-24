def test_class(lox_runner):
    # GIVEN
    code = """
    class Test {
        fun printA() {
            print "a";
        }
        fun printB() {
            print "b";
        }
    }

    var tInst = Test();
    tInst.printA();
    tInst.printB();
    """

    # WHEN
    stdout, stderr = lox_runner(code)

    # THEN
    assert stdout.strip().splitlines() == ["a", "b"]
    assert stderr == ""
