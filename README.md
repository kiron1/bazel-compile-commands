# `compile_commands.json` generator for Bazel

![bazel-compile-commands build status of main branch](https://github.com/kiron1/bazel-compile-commands/actions/workflows/main.yaml/badge.svg)

This repository contains two tools to generate a `compile_commands.json` file
from a Bazel workspace without the need to modify any of the Bazel configuration
files. This is the equivalent of _CMAKE_EXPORT_COMPILE_COMMANDS_ from CMake for
Bazel.

The
[Microsoft C/C++ Extension for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
struggles sometimes with Bazel workspaces since not all include paths are at the
default locations. The `compile_commands.json` file can be used with `clangd` to
have a significant better LSP experience. For
[Visual Studio Code (VS Code)](https://code.visualstudio.com), the
[clangd extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
can be used to utilize the project compile information via the
`compile_commands.json` file.

Invoke `bazel-compile-commands //...` to generate a `compile_commands.json` file
for the
[Bazel label](https://docs.bazel.build/versions/3.1.0/build-ref.html#labels)
`//...`. For more information, see
[`man 1 bazel-compile-commands`](./documentation.md).

The `bazel-compile-commands` command is ideal when you need a
`compile_commands.json` file instantly and don't require automatic updates of
the `compile_commands.json` file. For example in CI use cases when you when
static analysis tools use a `compile_commands.json` file as input.

## Usage of `bazel-compile-commands`

Inside a Bazel workspace run:

```sh
bazel-compile-commands
```

This will generate a `compile_commands.json` file in the current directory.

- [Documentation](./documentation.md) or `man 1 bazel-compile-commands`
- [Download latest release](https://github.com/kiron1/bazel-compile-commands/releases/latest)

### VS Code integration

See the [`.vscode/tasks.json`](./.vscode/tasks.json) file as an example on how
to integrate`bazel-compile-commands` into VS Code.

## Mentions

- [Bartek Kryza mentions `bazel-compile-commands` in the blog post
  "compile_commands.json gallery"](https://blog.bkryza.com/posts/compile-commands-json-gallery/),
  _Dec. 2, 2004_.
- [Blog post in System/5: BazelCon 2024 recap, section IDE support, bullet point
  "Compilation database"](https://blogsystem5.substack.com/p/bazelcon-2024-recap),
  _Oct. 22, 2024_.
- [BazelCon 2024: The State of Compilation Database in Bazel](https://www.youtube.com/watch?v=HJGD0-mX6G8&list=PLbzoR-pLrL6ptKfAQNZ5RS4HMdmeilBcw&index=41)
  (Video)

## Build

### Linux and macOS

```sh
bazel build --config=gnu //bcc:bazel-compile-commands
```

To cross-compiling for a Linux `aarch64` platform, run:

```sh
bazel build --extra_toolchains=@llvm_toolchain//:all --platforms=@toolchains_llvm//platforms:linux-aarch64 --config=gnu //bcc:bazel-compile-commands
```

### Windows

```sh
bazel build --config=cl //bcc:bazel-compile-commands
```

## Alternative tools

- [Bear](https://github.com/rizsotto/Bear) - Can work when used with the
  [`--spawn_strategy=local`](https://docs.bazel.build/versions/main/user-manual.html#flag--spawn_strategy)
  Bazel flag.
- [hedronvision/bazel-compile-commands-extractor](https://github.com/hedronvision/bazel-compile-commands-extractor) -
  Can be integrated into Bazel files.

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
