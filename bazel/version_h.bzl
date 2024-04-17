load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _version_h_impl(ctx):
    guard = "{}_{}_H_INCLUDED".format(ctx.label.name.upper(), ctx.attr.varname)
    content = [
        "#ifndef {}".format(guard),
        "#define {} \"{}\"".format(ctx.attr.varname, ctx.attr.version[BuildSettingInfo].value),
        "#endif",
    ]
    ctx.actions.write(
        output = ctx.outputs.out,
        content = "\n".join(content),
    )
    files = depset(direct = [ctx.outputs.out])
    runfiles = ctx.runfiles(files = [ctx.outputs.out])
    return [DefaultInfo(files = files, data_runfiles = runfiles)]

version_h = rule(
    implementation = _version_h_impl,
    output_to_genfiles = True,
    attrs = {
        "out": attr.output(mandatory = True),
        "varname": attr.string(
            doc = "Name of the varible to hold the versions",
        ),
        "version": attr.label(
            doc = "Version of this build.",
        ),
    },
    doc = "Version as C/C++ header file.",
)
