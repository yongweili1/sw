// Copyright (C) 2017-2018 Egor Pugin <egor.pugin@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "base.h"

namespace sw
{

// C#

struct SW_DRIVER_CPP_API CSharpTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<CSharpCompiler> compiler;

    TargetType getType() const override { return TargetType::CSharpLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API CSharpExecutable : CSharpTarget
{
    TargetType getType() const override { return TargetType::CSharpExecutable; }
};

// Rust

struct SW_DRIVER_CPP_API RustTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<RustCompiler> compiler;

    TargetType getType() const override { return TargetType::RustLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API RustExecutable : RustTarget
{
    TargetType getType() const override { return TargetType::RustExecutable; }
};

// Go

struct SW_DRIVER_CPP_API GoTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<GoCompiler> compiler;

    TargetType getType() const override { return TargetType::GoLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API GoExecutable : GoTarget
{
    TargetType getType() const override { return TargetType::GoExecutable; }
};

// Fortran

struct SW_DRIVER_CPP_API FortranTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<FortranCompiler> compiler;

    TargetType getType() const override { return TargetType::FortranLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API FortranExecutable : FortranTarget
{
    TargetType getType() const override { return TargetType::FortranExecutable; }
};

// Java

struct SW_DRIVER_CPP_API JavaTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<JavaCompiler> compiler;

    TargetType getType() const override { return TargetType::JavaLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API JavaExecutable : JavaTarget
{
    TargetType getType() const override { return TargetType::JavaExecutable; }
};

// Kotlin

struct SW_DRIVER_CPP_API KotlinTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<KotlinCompiler> compiler;

    TargetType getType() const override { return TargetType::KotlinLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API KotlinExecutable : KotlinTarget
{
    TargetType getType() const override { return TargetType::KotlinExecutable; }
};

// D

struct SW_DRIVER_CPP_API DTarget : Target
    , NativeTargetOptionsGroup
{
    SW_TARGET_USING_ASSIGN_OPS(NativeTargetOptionsGroup);

    std::shared_ptr<DCompiler> compiler;

    TargetType getType() const override { return TargetType::DLibrary; }

    bool init() override;
    Commands getCommands(void) const override;
    bool prepare() override;
    DependenciesType gatherDependencies() const override { return NativeTargetOptionsGroup::gatherDependencies(); }
    Files gatherAllFiles() const override { return NativeTargetOptionsGroup::gatherAllFiles(); }

    using TargetBase::operator+=;

private:
    using Target::getOutputFileName;
    path getOutputFileName(const path &root) const;
};

struct SW_DRIVER_CPP_API DLibrary : DTarget
{
};

struct SW_DRIVER_CPP_API DStaticLibrary : DLibrary
{
    TargetType getType() const override { return TargetType::DStaticLibrary; }
};

struct SW_DRIVER_CPP_API DSharedLibrary : DLibrary
{
    TargetType getType() const override { return TargetType::DSharedLibrary; }
};

struct SW_DRIVER_CPP_API DExecutable : DTarget
{
    TargetType getType() const override { return TargetType::DExecutable; }
};

}
