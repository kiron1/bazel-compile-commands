# `compile_commands.json` generator for Bazel

This `bazle-compile-commands` tool can generate a `compile_commands.json` file
without the need to modify any of the Bazel configuration files.

## Usage

Inside a Bazel workspace run:

```sh
bazel-compile-commands
```

This will generate a `compile_commands.json` file in the current directory.

## Alternative tools

- [Bear](https://github.com/rizsotto/Bear) - Can work when used with the [`--spawn_strategy=local`](https://docs.bazel.build/versions/main/user-manual.html#flag--spawn_strategy) Bazel flag.
- [hedronvision/bazel-compile-commands-extractor](https://github.com/hedronvision/bazel-compile-commands-extractor) - Can be integrated into Bazel files.
- [grailbio/bazel-compilation-database](https://github.com/grailbio/bazel-compilation-database) - Needs integration into your Bazel files

## Links

- [Format of the `compile_commands.json` format](https://clang.llvm.org/docs/JSONCompilationDatabase.html)
- [analysis_v2.proto](https://github.com/bazelbuild/bazel/blob/master/src/main/protobuf/analysis_v2.proto)
