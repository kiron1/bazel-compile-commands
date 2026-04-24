def _version_h_impl(ctx):
    guard = "{}_{}_H_INCLUDED".format(ctx.label.name.upper(), ctx.attr.varname)
    content = [
        "#ifndef {}".format(guard),
        "#define {} \"{}\"".format(ctx.attr.varname, ctx.attr.version),
        "#endif",
        "",
    ]
    ctx.actions.write(
        output = ctx.outputs.out,
        content = "\n".join(content),
    )
    files = depset(direct = [ctx.outputs.out])
    runfiles = ctx.runfiles(files = [ctx.outputs.out])
    return [DefaultInfo(files = files, data_runfiles = runfiles)]

_version_h = rule(
    implementation = _version_h_impl,
    output_to_genfiles = True,
    attrs = {
        "out": attr.output(mandatory = True),
        "varname": attr.string(
            doc = "Name of the varible to hold the versions",
        ),
        "version": attr.string(
            doc = "Version of this build.",
        ),
    },
    doc = "Version as C/C++ header file.",
)

def version_h(name, out, varname, **kwargs):
    _version_h(
        name = name,
        out = out,
        varname = varname,
        version = native.module_version(),
        **kwargs
    )
