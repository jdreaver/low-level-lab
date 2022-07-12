#!/usr/bin/env bash

set -u

tmp_dir=$(mktemp -d)

for input_file in tests/*.in; do
    test_name="$(basename "${input_file%.*}")" # Filename without extension
    echo "Testing $test_name"

    stdout_file="$tmp_dir/$test_name.stdout"
    stderr_file="$tmp_dir/$test_name.stderr"

    if ./main < "$input_file" > "$stdout_file" 2> "$stderr_file"; then
        expect_file="tests/$test_name.stdout"

        if ! [ -f "$expect_file" ]; then
            echo "Saw error in test $test_name, but couldn't find error file. Test stdout:" >&2
            cat "$stdout_file" >&2
            exit 1
        fi

        out_diff=$(diff "$expect_file" "$stdout_file")
        if [ -n "$out_diff" ]; then
            echo "Test $test_name failure. Found diff in stdout:" >&2
            echo "$out_diff" >&2
            exit 1
        fi
    else
        expect_file="tests/$test_name.stderr"

        if ! [ -f "$expect_file" ]; then
            echo "Saw error in test $test_name, but couldn't find error file. Test stderr:" >&2
            cat "$stderr_file" >&2
            echo "Test stdout:" >&2
            cat "$stdout_file" >&2
            exit 1
        fi

        err_diff=$(diff "$expect_file" "$stderr_file")
        if [ -n "$err_diff" ]; then
            echo "Test $test_name failure. Found diff in stderr:" >&2
            echo "$err_diff" >&2
            exit 1
        fi
    fi
done

echo "Tests passed"
