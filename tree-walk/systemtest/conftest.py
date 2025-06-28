import pytest

from pathlib import Path
import subprocess


@pytest.fixture
def lox_runner():
    BIN = Path(__file__).resolve().parent / "../../build/tree-walk/src/tree-walk"

    def run(code):
        result = subprocess.run([BIN, "-c", code], capture_output=True, text=True)
        return (result.stdout, result.stderr)

    return run
