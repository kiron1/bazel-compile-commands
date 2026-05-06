#!/usr/bin/env python3

import argparse
import pathlib
import json
import sys


def main():
    args = parse_args()
    compile_commands_path = args.build_path / "compile_commands.json"

    with compile_commands_path.open() as p:
        compile_commands = json.load(p)
    check(compile_commands)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-p",
        dest="build_path",
        type=pathlib.Path,
        default=pathlib.Path.cwd(),
        help="Build path",
    )
    return parser.parse_args()


def check(compile_commands):
    assert isinstance(compile_commands, list)
    assert len(compile_commands) > 0
    for c in compile_commands:
        assert isinstance(c["directory"], str)
        assert isinstance(c["arguments"], list)
        assert isinstance(c["file"], str)
        assert isinstance(c["output"], str)


if __name__ == "__main__":
    main()
