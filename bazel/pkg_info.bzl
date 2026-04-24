load("@rules_pkg//pkg:providers.bzl", "PackageVariablesInfo")

def _pkg_variables_impl(ctx):
    values = {
        "architecture": ctx.attr.architecture,
        "os": ctx.attr.os,
        "version": ctx.attr.version,
    }
    return PackageVariablesInfo(values = values)

_pkg_variables = rule(
    implementation = _pkg_variables_impl,
    attrs = {
        "architecture": attr.string(
            doc = "Architecture of this build.",
        ),
        "os": attr.string(
            doc = "OS of this build.",
        ),
        "version": attr.string(
            doc = "Version of this build.",
        ),
    },
    doc = "Collect variables used during package generation.",
)

def pkg_variables(name, architecture, os, **kwargs):
    _pkg_variables(
        name = name,
        architecture = architecture,
        os = os,
        version = native.module_version(),
        **kwargs
    )

def _pkg_version_impl(ctx):
    executable = ctx.actions.declare_file(ctx.label.name)
    exe_content = [
        "#!/bin/sh",
        "printf 'version=%s\n' \"{}\"".format(ctx.attr.version),
        "",
    ]
    ctx.actions.write(
        output = executable,
        content = "\n".join(exe_content),
    )
    ctx.actions.write(
        output = ctx.outputs.out,
        content = "{}\n".format(ctx.attr.version),
    )
    files = depset(direct = [ctx.outputs.out])
    runfiles = ctx.runfiles(files = [ctx.outputs.out])
    return [DefaultInfo(executable = executable, files = files, data_runfiles = runfiles)]

_pkg_version = rule(
    implementation = _pkg_version_impl,
    attrs = {
        "out": attr.output(mandatory = True),
        "version": attr.string(
            doc = "Version of this build.",
        ),
    },
    doc = "Version as file.",
    executable = True,
)

def pkg_version(name, **kwargs):
    _pkg_version(
        name = name,
        version = native.module_version(),
        target_compatible_with = select({
            "@platforms//os:linux": [],
            "@platforms//os:macos": [],
            "//conditions:default": ["@platforms//:incompatible"],
        }),
        **kwargs
    )
