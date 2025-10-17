# NAME

`bazel-clangd-wrapper` - run clangd with extra Bazel compatibility

# SYNOPSIS

<!-- deno-fmt-ignore-start -->

**bazel-clangd-wrapper** [**-h**]

**bazel-clangd-wrapper** [**--bazel-path** BAZEL-PATH] [**--clangd-path** CLANGD-PATH] [**-s** BAZEL-STARTUP-OPTION ] [ -- CLANGD-OPTIONS ]

<!-- deno-fmt-ignore-end -->

# DESCRIPTION

A wrapper for clangd that automatically handles Bazel build output paths.

Bazel runs build actions in a hermetic environment separate from the workspace
source tree. This results in **compile_commands.json** pointing to this output
directory rather than the workspace source tree.

bazel-clangd-wrapper queries Bazel to determine the output base and configures
clangd to associate the workspace source files with Bazel's output base.

# OPTIONS

--bazel-path **BAZEL-PATH**

: Path to the **bazel** executable. Default value is **bazel**. Can be set to
**bazelisk** to use bazelisk instead of bazel.

--clangd-path **CLANGD-PATH**

: Path to the **clangd** executable. Default value is **clangd**.

-h, --help

: Show the available options and exit.

-s, --bazelsupopt **OPTION**

: Additional Bazel startup options to be added to the `bazel` call.

# EXAMPLES

## Pass extra arguments to clangd

We can pass options to bazel-clangd-wrapper as well as the clangd server by using
`--` to separate the argument lists:

```sh
bazel-clangd-wrapper --bazel-path bazelisk -- --log=verbose --background-index
```

This sets the Bazel executable to `bazelisk`, while passing logging and indexing
options to clangd.

# SEE ALSO

bazel-compile-commands(1)
