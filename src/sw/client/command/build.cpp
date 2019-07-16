/*
 * SW - Build System and Package Manager
 * Copyright (C) 2017-2019 Egor Pugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "commands.h"

extern ::cl::opt<bool> build_after_fetch;

::cl::list<String> build_arg(::cl::Positional, ::cl::desc("Files or directories to build (paths to config)"), ::cl::sub(subcommand_build));

static ::cl::opt<String> build_source_dir("S", ::cl::desc("Explicitly specify a source directory."), ::cl::sub(subcommand_build), ::cl::init("."));
static ::cl::opt<String> build_binary_dir("B", ::cl::desc("Explicitly specify a build directory."), ::cl::sub(subcommand_build), ::cl::init(SW_BINARY_DIR));

static ::cl::opt<bool> build_fetch("fetch", ::cl::desc("Fetch sources, then build"), ::cl::sub(subcommand_build));

////////////////////////////////////////////////////////////////////////////////
//
// build configs
//
////////////////////////////////////////////////////////////////////////////////

//static cl::opt<bool> append_configs("append-configs", cl::desc("Append configs for generation"));

static cl::list<String> libc("libc", cl::CommaSeparated);
static cl::list<String> libcpp("libcpp", cl::CommaSeparated);
static cl::list<String> target_os("target-os", cl::CommaSeparated);
static cl::list<String> compiler("compiler", cl::desc("Set compiler"), cl::CommaSeparated);
static cl::list<String> configuration("configuration", cl::desc("Set build configuration"), cl::CommaSeparated);
cl::alias configuration2("config", cl::desc("Alias for -configuration"), cl::aliasopt(configuration));
static cl::list<String> platform("platform", cl::desc("Set build platform"), cl::CommaSeparated);
cl::alias platform2("arch", cl::desc("Alias for -platform"), cl::aliasopt(platform));
static cl::list<String> os("os", cl::desc("Set build target os"), cl::CommaSeparated);

// static/shared
static cl::opt<bool> static_build("static-build", cl::desc("Set static build"));
cl::alias static_build2("static", cl::desc("Alias for -static-build"), cl::aliasopt(static_build));
static cl::opt<bool> shared_build("shared-build", cl::desc("Set shared build (default)"));
cl::alias shared_build2("shared", cl::desc("Alias for -shared-build"), cl::aliasopt(shared_build));

//mt/md
static cl::opt<bool> win_mt("win-mt", cl::desc("Set /MT build"));
cl::alias win_mt2("mt", cl::desc("Alias for -win-mt"), cl::aliasopt(win_mt));
static cl::opt<bool> win_md("win-md", cl::desc("Set /MD build (default)"));
cl::alias win_md2("md", cl::desc("Alias for -win-md"), cl::aliasopt(win_md));

////////////////////////////////////////////////////////////////////////////////

SUBCOMMAND_DECL(build)
{
    auto swctx = createSwContext();
    cli_build(*swctx);
}

static bool hasAnyUserProvidedInformation()
{
    return 0
        || !configuration.empty()
        || static_build
        || shared_build
        || win_mt
        || win_md
        || !platform.empty()
        || !compiler.empty()
        || !target_os.empty()
        || !libc.empty()
        ;

    //|| (static_build && shared_build) // when both; but maybe ignore?
    //|| (win_mt && win_md) // when both; but maybe ignore?

}

static bool hasUserProvidedInformationStrong()
{
    return 0
        || !configuration.empty()
        || !compiler.empty()
        || !target_os.empty()
        ;
}

static sw::TargetSettings compilerTypeFromStringCaseI(const sw::UnresolvedPackage &compiler)
{
    sw::TargetSettings ts;

    auto with_version = [&compiler](const auto &ppath)
    {
        return sw::UnresolvedPackage(ppath, compiler.range);
    };

    auto set_with_version = [&with_version](const auto &ppath)
    {
        return with_version(ppath).toString();
    };

    if (0);
    /*
    // starts with
    else if (boost::istarts_with(compiler, "appleclang") || boost::iequals(compiler, "apple-clang"))
        return CompilerType::AppleClang;
    else if (boost::istarts_with(compiler, "gnu") || boost::iequals(compiler, "gcc") || boost::iequals(compiler, "g++"))
        return CompilerType::GNU;*/
    else if (compiler.ppath == "clang")
    {
        ts["native"]["program"]["c"] = set_with_version("org.LLVM.clang");
        ts["native"]["program"]["cpp"] = set_with_version("org.LLVM.clangpp");
        ts["native"]["program"]["asm"] = set_with_version("org.LLVM.clang"); // llvm-as?
        ts["native"]["program"]["lib"] = set_with_version("org.LLVM.ar");
        ts["native"]["program"]["link"] = set_with_version("org.LLVM.lld");
    }
    else if (compiler.ppath == "clangcl"/* || compiler.ppath == "clang-cl"*/)
    {
        ts["native"]["program"]["c"] = set_with_version("org.LLVM.clangcl");
        ts["native"]["program"]["cpp"] = set_with_version("org.LLVM.clangcl");
    }
    else if (compiler.ppath == "msvc" || compiler.ppath == "vs")
    {
        ts["native"]["program"]["c"] = set_with_version("com.Microsoft.VisualStudio.VC.cl");
        ts["native"]["program"]["cpp"] = set_with_version("com.Microsoft.VisualStudio.VC.cl");
        ts["native"]["program"]["asm"] = set_with_version("com.Microsoft.VisualStudio.VC.ml");
    }
    else
    {
        ts["native"]["program"]["c"] = compiler.toString();
        ts["native"]["program"]["cpp"] = compiler.toString();
        if (compiler.ppath == "com.Microsoft.VisualStudio.VC.cl")
            ts["native"]["program"]["asm"] = set_with_version("com.Microsoft.VisualStudio.VC.ml");
    }
    return ts;
}

static String configurationTypeFromStringCaseI(const String &in)
{
    auto configuration = boost::to_lower_copy(in);
    if (configuration == "d")
        return "debug";
    else if (configuration == "r")
        return "release";
    else if (
        configuration == "minsizerel" ||
        configuration == "msr")
        return "minimalsizerelease";
    else if (configuration == "relwithdebinfo" ||
        configuration == "rwdi" ||
        configuration == "releasewithdebinfo")
        return "releasewithdebuginformation";
    return configuration;
}

static String OSTypeFromStringCaseI(const String &in)
{
    auto target_os = boost::to_lower_copy(in);
    if (target_os == "win" || target_os == "windows")
        return "com.Microsoft.Windows.NT";
    return target_os;
}

static String archTypeFromStringCaseI(const String &in)
{
    auto platform = boost::to_lower_copy(in);
    if (platform == "win32" ||
        platform == "x86")
        return "x86";
    else if (
        platform == "win64" ||
        platform == "x64" ||
        platform == "x64_86")
        return "x86_64";
    else if (platform == "arm32")
        return "arm";
    else if (platform == "arm64")
        return "aarch64";
    return platform;
}

static String osTypeFromStringCaseI(const String &in)
{
    auto os = boost::to_lower_copy(in);
    if (os == "win" || os == "windows")
        return "com.Microsoft.Windows.NT";
    else if (os == "linux")
        return "org.torvalds.linux";
    else if (os == "mac" || os == "macos")
        return "com.Apple.Macos"; // XNU? Darwin?
    return os;
}

std::vector<sw::TargetSettings> create_settings(const sw::SwCoreContext &swctx)
{
    std::vector<sw::TargetSettings> settings;
    settings.push_back(swctx.getHostSettings());

    if (!hasAnyUserProvidedInformation())
        return settings;

    auto times = [&settings](int n)
    {
        if (n <= 1)
            return;
        auto s2 = settings;
        for (int i = 1; i < n; i++)
        {
            for (auto &s : s2)
                settings.push_back(s);
        }
    };

    auto mult_and_action = [&settings, &times](int n, auto f)
    {
        times(n);
        for (int i = 0; i < n; i++)
        {
            int mult = settings.size() / n;
            for (int j = i * mult; j < (i + 1) * mult; j++)
                f(*(settings.begin() + j), i);
        }
    };

    // configuration
    Strings configs;
    for (auto &c : configuration)
    {
        /*if (used_configs.find(c) == used_configs.end())
        {
            if (isConfigSelected(c))
                LOG_WARN(logger, "config was not used: " + c);
        }
        if (!isConfigSelected(c))*/
            configs.push_back(c);
    }
    mult_and_action(configs.size(), [&configs](auto &s, int i)
    {
        s["native"]["configuration"] = configurationTypeFromStringCaseI(configs[i]);
    });

    // static/shared
    if (static_build && shared_build)
    {
        mult_and_action(2, [](auto &s, int i)
        {
            if (i == 0)
                s["native"]["library"] = "static";
            if (i == 1)
                s["native"]["library"] = "shared";
        });
    }
    else
    {
        for (auto &s : settings)
        {
            if (static_build)
                s["native"]["library"] = "static";
            if (shared_build)
                s["native"]["library"] = "shared";
        }
    }

    // mt/md
    if (win_mt && win_md)
    {
        mult_and_action(2, [&settings](auto &s, int i)
        {
            if (i == 0)
                s["native"]["mt"] = "true";
        });
    }
    else
    {
        for (auto &s : settings)
        {
            if (win_mt)
                s["native"]["mt"] = "true";
        }
    }

    // platform
    mult_and_action(platform.size(), [](auto &s, int i)
    {
        s["os"]["arch"] = archTypeFromStringCaseI(platform[i]);
    });

    // os
    mult_and_action(os.size(), [](auto &s, int i)
    {
        s["os"]["kernel"] = osTypeFromStringCaseI(os[i]);
    });

    // compiler
    mult_and_action(compiler.size(), [](auto &s, int i)
    {
        s.merge(compilerTypeFromStringCaseI(compiler[i]));
    });

    // target_os
    mult_and_action(target_os.size(), [](auto &s, int i)
    {
        s["os"]["kernel"] = OSTypeFromStringCaseI(target_os[i]);
    });

    // libc
    //auto set_libc = [](auto &s, const String &libc)
    //{
    //    s.Settings.Native.libc = libc;
    //};

    //mult_and_action(libc.size(), [&set_libc](auto &s, int i)
    //{
    //    set_libc(s, libc[i]);
    //});

    return settings;
}

SUBCOMMAND_DECL2(build)
{
    if (build_fetch)
    {
        build_after_fetch = true;
        return cli_fetch(swctx);
    }

    // defaults or only one of build_arg and -S specified
    //  -S == build_arg
    //  -B == fs::current_path()

    // if -S and build_arg specified:
    //  source dir is taken as -S, config dir is taken as build_arg

    // if -B specified, it is used as is

    for (auto &a : build_arg)
    {
        auto &i = swctx.addInput(a);
        for (auto &s : create_settings(swctx))
            i.addSettings(s);
    }
    swctx.build();
}
