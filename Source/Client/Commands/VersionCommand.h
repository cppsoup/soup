﻿// <copyright file="VersionCommand.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "ICommand.h"
#include "VersionOptions.h"

namespace Soup::Client
{
    /// <summary>
    /// Version Command
    /// </summary>
    class VersionCommand : public ICommand
    {
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="VersionCommand"/> class.
        /// </summary>
        VersionCommand(VersionOptions options) :
            m_options(std::move(options))
        {
        }

        /// <summary>
        /// Main entry point for a unique command
        /// </summary>
        virtual void Run() override final
        {
            Log::Trace("VersionsCommand::Run");
            // var version = Assembly.GetExecutingAssembly().GetName().Version;
            // Log.Info($"{version.Major}.{version.Minor}.{version.Build}");
            // return Task.CompletedTask;
        }

    private:
        VersionOptions m_options;
    };
}
