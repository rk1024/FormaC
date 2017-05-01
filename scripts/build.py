import fnmatch
import glob
import itertools as it
import logging as l
import subprocess as sp
import sys
from os import path

import NinjaSnek.configure as conf

l.basicConfig(level = l.DEBUG)


def pkgConfig(*args):
  pinf = ["pkg-config"]
  pinf.extend(args)

  p = sp.Popen(pinf, stdout = sp.PIPE)

  o, _ = p.communicate()

  r = p.wait()

  if r: raise RuntimeError("pkg-config exited with code {}.".format(r))

  return [flag.strip() for flag in str(o).split(" ")]


def pkcflags(*args):
  return pkgConfig("--cflags", *args)


def pklibs(*args):
  return pkgConfig("--libs", *args)


def flatten(*args):
  return list(it.chain(*args))


def main():
  build = conf.Build()

  debug = True

  cxxflags = [
    "-fcolor-diagnostics",
    "-fno-elide-type",
    "-std=c++14",
    "-Wall",
    "-Wconversion",
    "-Wdeprecated",
    "-Wextra",
    "-Wimplicit",
    "-Winvalid-noreturn",
    "-Wmissing-noreturn",
    "-Wmissing-prototypes",
    "-Wmissing-variable-declarations",
    "-Wnewline-eof",
    "-Wshadow",
    "-Wno-shorten-64-to-32",
    "-Wno-sign-compare",
    "-Wno-sign-conversion",
    "-Wthread-safety",
    "-Wunreachable-code-aggressive",
    "-Wunused",
    "-Werror=old-style-cast",
    "-Werror=return-type",
  ]

  ldflags = []

  flexflags = [
    "--ecs",
    "--full",
  ]

  bisonflags = [
    "--warnings=all",
    "--report=all",
  ]

  astgenFlags = [
    "-f$srcdir/flex-test.in.lpp:$builddir/flex-test.lpp",
    "-b$srcdir/bison-test.in.ypp:$builddir/bison-test.ypp"
  ]

  astgenFlagsInternal = [
    flag.replace("$srcdir", "src").replace("$builddir", "build")
    for flag in astgenFlags
  ]

  def addPkgs(*args):
    cxxflags.extend(pkcflags(*args))
    ldflags.extend(pklibs(*args))

  if debug:
    sanflags = ["-fsanitize=%s" % (san) for san in [
      "address",
      "undefined",
    ]]

    cxxflags.extend(sanflags)
    ldflags.extend(sanflags)

    cxxflags.extend([
      "-g",
      "-O0",
      "-D_GLIBCXX_DEBUG",
      "-D_LIBCPP_DEBUG",
      "-DDEBUG",
      "-D_DEBUG",
    ])
  else:
    cxxflags.extend([
      "-Ofast",
      "-DNDEBUG",
      "-D_NDEBUG",
    ])

  l.debug(
    "CXX Flags: {}".format(
      " ".join(
        "'{}'".format(str(flag)) if " " in str(flag) else str(flag)
        for flag in cxxflags
      )
    )
  )
  l.debug(
    "LD  Flags: {}".format(
      " ".join(
        "'{}'".format(str(flag)) if " " in str(flag) else str(flag)
        for flag in ldflags
      )
    )
  )

  build.set(
    cxx = "clang++",
    flex = "flex",
    bison = "bison",
    ruby = "ruby",
    cxxflags = " ".join(cxxflags),
    ldflags = " ".join(ldflags),
    flexflags = " ".join(flexflags),
    bisonflags = " ".join(bisonflags),
    astgenFlags = " ".join(astgenFlags),
    srcdir = build.path("src"),
    bindir = build.path("bin"),
  )

  build.rule(
    "cxx", targets = (".o"), deps = (".cpp")
  ).set(
    command = "$cxx $cxxflags $flags -MMD -MF $out.d -c -o $out $in",
    deps = "gcc",
    depfile = "$out.d",
  )

  build.rule(
    "link", targets = (""), deps = (".o")
  ).set(
    command = "$cxx $ldflags $flags -o $out $in",
  )

  build.rule(
    "flex", targets = (".cpp"), deps = (".lpp")
  ).set(
    command = "$flex $flexflags $flags -o $out $in",
  )

  build.rule(
    "bison", targets = (".cpp"), deps = (".ypp")
  ).set(
    command = "$bison $bisonflags $flags -o $out $in",
  )

  build.rule("ruby").set(
    command = "$ruby $rubyflags $flags $in $args",
  )

  build.edges(
    (build.path_b("flex-test.cpp"), "$builddir/flex-test.lpp"),
    (([build.path_b("bison-test.cpp")], build.paths_b(
      *[
        "bison-test.hpp", "bison-test.output", "location.hh", "position.hh",
        "stack.hh"
      ]
    )), "$builddir/bison-test.ypp"),
    ("parse-test", "phony", "$bindir/parse-test"),
  )

  astSources = [
    path.relpath(src.strip(), "build")
    for src in str(
      sp.check_output(
        flatten(["ruby", "scripts/astgen.rb", "-l", "build/ast"],
                astgenFlagsInternal)
      )
    ).split(" ")
  ]

  astImplSources = [
    path.relpath(src.strip(), "build")
    for src in str(
      sp.check_output(
        flatten(["ruby", "scripts/astgen.rb", "-i", "build/ast"],
                astgenFlagsInternal)
      )
    ).split(" ")
  ]

  sources = {
    "$bindir/parse-test": (
      flatten(
        ["parseTest.cpp"],
        (path.relpath(p, "src") for p in glob.glob("src/ast/*.cpp")),
      ), flatten(
        ["flex-test.cpp", "bison-test.cpp"],
        fnmatch.filter(astSources, "**/*.cpp"),
      )
    )
  }

  l.debug(sources)
  l.debug(astImplSources)

  build.edge(
    (build.paths_b(*astSources), build.paths_b(*astImplSources)), "ruby",
    build.path("scripts/astgen.rb")
  ).set(args = "$astgenFlags $rootdir/build/ast")

  for out, (ins, b_ins) in sources.iteritems():
    for n in ins:
      build.edge(
        build.path_b("{}.o".format(path.splitext(n)[0])),
        "$srcdir/{}".format(n)
      ).set(flags = " ".join(["-I$builddir -I$srcdir"]))

    for b in b_ins:
      build.edge(
        build.path_b("{}.o".format(path.splitext(b)[0])), build.path_b(b)
      ).set(flags = " ".join(["-I$builddir -I$srcdir"]))

    build.edge(
      out,
      build.paths_b(
        *["{}.o".format(path.splitext(n)[0]) for n in it.chain(ins, b_ins)]
      )
    )

  return build.run(".", "build", *sys.argv[1:])


sys.exit(main())
