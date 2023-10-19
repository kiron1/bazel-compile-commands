load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@rules_pkg//pkg:providers.bzl", "PackageVariablesInfo")

def _pkg_variables_impl(ctx):
    values = {
        "architecture": ctx.attr.architecture,
        "os": ctx.attr.os,
        "version": ctx.attr.version[BuildSettingInfo].value,
    }
    return PackageVariablesInfo(values = values)

pkg_variables = rule(
    implementation = _pkg_variables_impl,
    attrs = {
        "architecture": attr.string(
            doc = "Architecture of this build.",
        ),
        "os": attr.string(
            doc = "OS of this build.",
        ),
        "version": attr.label(
            doc = "Version of this build.",
        ),
    },
    doc = "Collect variables used during package generation.",
)

def _pkg_version_impl(ctx):
    ctx.actions.write(
        output = ctx.outputs.out,
        content = "{}\n".format(ctx.attr.version[BuildSettingInfo].value),
    )
    files = depset(direct = [ctx.outputs.out])
    runfiles = ctx.runfiles(files = [ctx.outputs.out])
    return [DefaultInfo(files = files, data_runfiles = runfiles)]

pkg_version = rule(
    implementation = _pkg_version_impl,
    attrs = {
        "out": attr.output(mandatory = True),
        "version": attr.label(
            doc = "Version of this build.",
        ),
    },
    doc = "Version as file.",
)
