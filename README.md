# `compile_commands.json` generator for Bazel

![bazel-compile-commands build status of main branch](https://github.com/kiron1/bazel-compile-commands/actions/workflows/main.yaml/badge.svg)

This `bazle-compile-commands` tool can generate a `compile_commands.json` file
from a Bazel workspace without the need to modify any of the Bazel
configuration files. This is the equivalent of _CMAKE_EXPORT_COMPILE_COMMANDS_
from CMake for Bazel.

## Usage

Inside a Bazel workspace run:

```sh
bazel-compile-commands
```

This will generate a `compile_commands.json` file in the current directory.

- [Documentation](./documentation.md) or `man 1 bazel-compile-commands`
- [Download latest release](https://github.com/kiron1/bazel-compile-commands/releases/latest)

## Alternative tools

- [Bear](https://github.com/rizsotto/Bear) - Can work when used with the
  [`--spawn_strategy=local`](https://docs.bazel.build/versions/main/user-manual.html#flag--spawn_strategy)
  Bazel flag.
- [hedronvision/bazel-compile-commands-extractor](https://github.com/hedronvision/bazel-compile-commands-extractor) -
  Can be integrated into Bazel files.
- [grailbio/bazel-compilation-database](https://github.com/grailbio/bazel-compilation-database) -
  Needs integration into your Bazel files

## Links

- [Format of the `compile_commands.json` format](https://clang.llvm.org/docs/JSONCompilationDatabase.html)
- [analysis_v2.proto](https://github.com/bazelbuild/bazel/blob/master/src/main/protobuf/analysis_v2.proto)

## Issues

- https://github.com/bazelbuild/bazel/issues/12852

## License

This source code is under the [MIT](https://opensource.org/licenses/MIT) license
with the exceptions mentioned in "Third party source code in this repository".
