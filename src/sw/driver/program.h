// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2019-2020 Egor Pugin <egor.pugin@gmail.com>

#pragma once

#include <sw/builder/node.h>

#include <primitives/filesystem.h>

#define SW_DECLARE_PROGRAM_CLONE \
    std::unique_ptr<Program> clone() const override

#define SW_DEFINE_PROGRAM_CLONE(t)            \
    std::unique_ptr<Program> t::clone() const \
    {                                         \
        return std::make_unique<t>(*this);    \
    }

#define SW_DEFINE_PROGRAM_CLONE_INLINE(t)           \
    std::unique_ptr<Program> clone() const override \
    {                                               \
        return std::make_unique<t>(*this);          \
    }

namespace sw
{

struct Build;
struct SourceFile;
struct Target;

struct SW_DRIVER_CPP_API Program : ICastable, detail::Executable
{
    path file;

    Program();
    Program(const Program &);
    Program &operator=(const Program &);
    virtual ~Program() = default;

    virtual std::unique_ptr<Program> clone() const = 0;
};

using ProgramPtr = std::unique_ptr<Program>;

// reconsider
struct SW_DRIVER_CPP_API PredefinedProgram
{
    void setProgram(ProgramPtr &&p) { program = std::move(p); }
    Program &getProgram();
    const Program &getProgram() const;

private:
    ProgramPtr program;
};

struct ProgramGroup : Program
{
    using Program::Program;

    std::shared_ptr<builder::Command> getCommand() const override { return nullptr; }
    //Version &getVersion() override { SW_UNIMPLEMENTED; }

    virtual void activate(Build &s) const = 0;
};

using ProgramGroupPtr = std::shared_ptr<ProgramGroup>;

}
