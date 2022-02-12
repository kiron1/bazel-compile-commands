# NAME

bazel-compile-commands - generate a compile_commands.json file from a Bazel
workspace

# SYNOPSIS

**bazel-compile-commands** [**-h**] **bazel-compile-commands** [**-av**] [**-b**
BAZEL-OPTION] [**-B** BAZEL-COMMAND] [**-c** COMPILER] [**-o** OUTPUT-FILE]
[**-s** BAZEL-STARTUP-OPTION] [**TARGETS**]

# DESCRIPTION

A non-intrusive way to generate a **compile\_commands.json** file from a Bazel
workspace. This is basically the **CMAKE_EXPORT_COMPILE_COMMANDS** option from
CMake for Bazel.

This works by issuing a bazel **aquery** and parsing this output and converting
into the compile commands JSON format.

When bazel-compile-commands is called it searches for a **.bazelccrc** file in
the current directory or any parent directory. This file can be used to set
default values for the arguments. The provided arguments via command line and
from the configuration file are combined when bazel-compile-commands is called

# OPTIONS

-a, --arguments

: Also write the array of arguments for each entry in the
`compile_commands.json` file.

-B, --bazel-command **BAZEL**

: Path to the **bazel** executable. Default value is **bazel**. Can be set to
**bazelisk** to use bazelisk instead of bazel.

-b, --bazelopt **OPTION**

-c, --compiler **FILE**

: Replace the internal Bazel compiler wrapper script with this compiler (useful
for macOS).

-h, --help

: Show the available options and exit.

-o, --output **FILE**

: Output file path of the `compile_commands.json` file. The default value is
**%workspace%/compile_commands.json** the `%workspace%` part will be replaced
with the actual worspace path which will be the output of
`bazel info workspace`.

-s, --bazelsupopt **OPTION**

: Additional Bazel startup options to be added to the `bazel` call.

-v, --verbose

: Be more verbose and print additional informations.

-w, --write

: Write the current supplied arguments into a **.bazelccrc** file such that
further calls to bazel-compile-commands without arguments behaves the same as if
the arguments are provided.

If a .bazelccrc file aready exists this file will be updated. When no surch file
exists, a new file in the current Bazel workspace will be created.

**TARGETS**

: All remaining arguments are expected to be Bazel **targets** for which the
compile commands shall be generated. The default is **//...** to generate the
compile commands for all C/C++ files.

# FILES

bazel-compile-commands will search for a **.bazelccrc** file in the current
directory or any parent directory. If such a file is found this values are used
and combined with the values from the command line.

An example **.bazelccrc** will look like:

```
verbose = 0
arguments = 0
bazel-command = bazel
output = %workspace%/compile_commands.json
targets = //...
```
