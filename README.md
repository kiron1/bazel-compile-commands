# `compile_commands.json` generator for Bazel

![bazel-compile-commands build status of main branch](https://github.com/kiron1/bazel-compile-commands/actions/workflows/main.yaml/badge.svg)

This repository contains two tools to generate a `compile_commands.json` file
from a Bazel workspace without the need to modify any of the Bazel configuration
files. This is the equivalent of _CMAKE_EXPORT_COMPILE_COMMANDS_ from CMake for
Bazel.

The
[Microsoft C/C++ Extension for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
struggels sometimes with Bazel workspaces since not all include paths are at the
default locations. The `compile_commands.json` file can be used with `clangd` to
have a significant better LSP experience. For [Visual Studio Code (VS
Code)][vscode], the [clangd extension][llvm-vs-code-extensions.vscode-clangd]
can be used to utilize the project compile information via the
`compile_commands.json` file.

`bazel-compile-commands`

: Invoke `bazel-compile-commands //...` to generate a `compile_commands.json`
file for the
[Bazel label](https://docs.bazel.build/versions/3.1.0/build-ref.html#labels)
`//...`. For more information, see
[`man 1 bazel-compile-commands`](./documentation.md).

`basil-tonic`

: Basil Tonic is a service which can listen to
[Bazel Build Events](https://bazel.build/remote/bep) and will write a
`compile_commands.json` to the Bazel workspace root after compilation finished.

The `bazel-compile-commands` command is ideal when you need a
`compile_commands.json` file instandlty and don't require automatic updates of
the `compile_commands.json` file. For example in CI use cases when you when
static analysis tools use a `compile_commands.json` file as input.

On the other hand `basil-tonic` is best suited for IDE use cases where the
`compile_commands.json` file is automaitcally updated with each build.

## Usage of `bazel-compile-commands`

Inside a Bazel workspace run:

```sh
bazel-compile-commands
```

This will generate a `compile_commands.json` file in the current directory.

- [Documentation](./documentation.md) or `man 1 bazel-compile-commands`
- [Download latest release](https://github.com/kiron1/bazel-compile-commands/releases/latest)

## Usage of Basil Tonic

See [the dedicated README for Basil Tonic](./basil_tonic/README.md).

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
- [clangd](https://clangd.llvm.org/)
- [analysis_v2.proto](https://github.com/bazelbuild/bazel/blob/master/src/main/protobuf/analysis_v2.proto)
- [Build Event Protocol](https://bazel.build/remote/bep)

## Issues

- https://github.com/bazelbuild/bazel/issues/12852

## License

This source code is under the [MIT](https://opensource.org/licenses/MIT) license
with the exceptions mentioned in "Third party source code in this repository".
