# Basil Tonic

Automatically generates `compile_command.json` files for an Bazel workspace
without modifications.

## Installation

- Download the latest Debian package from
  https://github.com/kiron1/bazel-compile-commands/releases
- Install via:
  ```sh
  sudo dpkg --install bazel-compile-commands_*_amd64.deb
  ```
- Enable service:
  ```sh
  systemctl --user enable --now basil-tonic.socket
  ```

## Configure Bazel

Ensure when building a C/C++ workspace, that Bazel will send build events to
Basil Tonic.

Either add the relevant arguments when calling `bazel build ...` or add them to
one of the `.bazelrc` [files loaded by Bazel during startup][bazelrc].

For example in the `$HOME/.bazelrc` file:

```
build:basil-tonic --bes_backend=grpc://127.0.0.1:50151
build:basil-tonic --build_event_publish_all_actions
build:basil-tonic --nobes_lifecycle_events
build:basil-tonic --bes_timeout=2s

# The line below ensures that basil-tonic is used by default.
# Uncomment the line and use `bazel build --config=basil-tonic`
# to only use it on selected build invocations.
build --config=basil-tonic
```

## Configure VSCode

for [Visual Studio Code (VS Code)][vscode], use the
[clangd extension][llvm-vs-code-extensions.vscode-clangd]. `clangd` will
automatically pick up the `compile_commands.json` from the workspace root.

[vscode]: https://code.visualstudio.com/ "Visual Studio Code"
[llvm-vs-code-extensions.vscode-clangd]: https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd "clangd - C/C++ completion, navigation, and insights"
[bazelrc]: https://bazel.build/run/bazelrc "Write bazelrc configuration files"

## Usage workflow

When building a Bazel workspace, Bazel will send build events to Basil Tonic.
Basil Tonic will use the information from the build events protocol to build the
build commands database and extend the existing `compile_commands.json` file if
exists. The final `compile_commands.json` file will be written, when the build
is finished.

<!-- deno-fmt-ignore-start -->

> **Note**
> You might need to restart `clangd` when you started it _before_ any
> `compile_commands.json` existed.

> **Note**
> Bazel will only send build events for files it is compiling. If you buid your
> workspace already before using Basil Tonic Bazel will use the cached results
> and not compile the C/C++ files. Either run `bazel clean` to force a re-build
> of the workspace or use `bazel-copmpile-commands` to generate an initial
> `compile_commands.json` file.

<!-- deno-fmt-ignore-end -->
