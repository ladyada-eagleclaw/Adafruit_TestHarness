"""Exercise discovery and failure reporting using the real host compiler."""
import contextlib
import importlib.util
import io
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location(
    "harness", Path(__file__).resolve().parents[1] / "run_tests.py")
harness = importlib.util.module_from_spec(spec)
spec.loader.exec_module(harness)


class RunnerTests(unittest.TestCase):
    def test_empty_suite_fails(self):
        with self.assertRaises(ValueError):
            harness.run_tests([])

    def test_failure_does_not_skip_remaining_tests(self):
        with tempfile.TemporaryDirectory(prefix="harness-runner-") as folder:
            root = Path(folder)
            bad = root / "a_bad.cpp"
            good = root / "b_good.cpp"
            bad.write_text("int main() { return 1; }\n")
            good.write_text("int main() { return 0; }\n")
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                with self.assertRaises(SystemExit) as error:
                    harness.run_tests([bad, good])
            self.assertEqual(error.exception.code, 1)
            self.assertIn("PASS " + str(good), output.getvalue())
            self.assertIn("1/2 test sources passed", output.getvalue())

    def test_compilation_failure_fails_suite(self):
        with tempfile.TemporaryDirectory(prefix="harness-compile-") as folder:
            source = Path(folder) / "broken.cpp"
            source.write_text('#error "intentional compiler failure"\n')
            with self.assertRaises(SystemExit) as error:
                harness.run_tests([source])
            self.assertEqual(error.exception.code, 1)


if __name__ == "__main__":
    unittest.main()
