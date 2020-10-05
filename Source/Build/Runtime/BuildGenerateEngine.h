﻿// <copyright file="BuildGenerateEngine.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "System/BuildState.h"
#include "System/BuildSystem.h"
#include "System/ValueTable.h"
#include "OperationGraph/OperationGraphGenerator.h"
#include "OperationGraph/OperationGraphManager.h"

namespace Soup::Build::Runtime
{
	export class BuildGenerateResult
	{
	public:
		BuildGenerateResult(
			OperationGraph graph,
			ValueTable sharedState) :
			Graph(std::move(graph)),
			SharedState(std::move(sharedState))
		{
		}

		OperationGraph Graph;
		ValueTable SharedState;
	};

	/// <summary>
	/// The core build generate engine that knows how to run all of the required build Tasks to generate the
	/// Operation Graph.
	/// </summary>
	export class BuildGenerateEngine
	{
	public:
		static Path GetOutputDirectory(
			std::string_view compiler,
			std::string_view flavor,
			std::string_view system,
			std::string_view architecture)
		{
			// Setup the output directories
			return Path("out") +
				Path(compiler) +
				Path(flavor) +
				Path(system) +
				Path(architecture);
		}

		static Path GetObjectDirectory(
			std::string_view compiler,
			std::string_view flavor,
			std::string_view system,
			std::string_view architecture)
		{
			// Setup the object directories
			return GetOutputDirectory(compiler, flavor, system, architecture) + Path("obj");
		}

		static Path GetBinaryDirectory(
			std::string_view compiler,
			std::string_view flavor,
			std::string_view system,
			std::string_view architecture)
		{
			// Setup the binary directories
			return GetOutputDirectory(compiler, flavor, system, architecture) + Path("bin");
		}

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="BuildGenerateEngine"/> class.
		/// </summary>
		BuildGenerateEngine(
			FileSystemState& fileSystemState,
			std::string systemCompiler) :
			_fileSystemState(fileSystemState)
		{
		}

		/// <summary>
		/// Execute the entire operation graph that is referenced by this build generate engine
		/// </summary>
		BuildGenerateResult Generate(
			std::string_view activeCompiler,
			std::string_view flavor,
			std::string_view system,
			std::string_view architecture,
			const Path& packageRoot,
			ValueTable& activeState,
			const std::vector<Path>& buildExtensionLibraries)
		{
			// Run all build operations in the correct order with incremental build checks
			Log::Diag("Build generate start");
			auto result = CheckExecuteGenerate(
				activeCompiler,
				flavor,
				system,
				architecture,
				packageRoot,
				activeState,
				buildExtensionLibraries);
			Log::Diag("Build generate end");

			return result;
		}

	private:
		/// <summary>
		/// Execute the collection of build operations
		/// </summary>
		BuildGenerateResult CheckExecuteGenerate(
			std::string_view activeCompiler,
			std::string_view flavor,
			std::string_view system,
			std::string_view architecture,
			const Path& packageRoot,
			ValueTable& activeState,
			const std::vector<Path>& buildExtensionLibraries)
		{
			auto outputDirectory = GetOutputDirectory(
				activeCompiler,
				flavor,
				system,
				architecture);
			auto targetDirectory = packageRoot + outputDirectory;

			// Load the previous build graph
			Log::Diag("Loading previous build graph");
			auto previousOperationGraph = OperationGraph(0);
			if (!OperationGraphManager::TryLoadState(
				targetDirectory,
				previousOperationGraph,
				_fileSystemState.GetId()))
			{
				Log::Info("No valid previous build graph found");
			}

			auto generateResult = RunEvaluate(
				activeCompiler,
				flavor,
				system,
				architecture,
				packageRoot,
				activeState,
				buildExtensionLibraries);

			Log::Diag("Merge previous operation graph observed results");
			for (auto& activeOperationEntry : generateResult.Graph.GetOperations())
			{
				auto& activeOperationInfo = activeOperationEntry.second;
				OperationInfo* previousOperationInfo = nullptr;
				if (previousOperationGraph.TryFindOperationInfo(activeOperationInfo.Command, previousOperationInfo))
				{
					activeOperationInfo.WasSuccessfulRun = previousOperationInfo->WasSuccessfulRun;
					activeOperationInfo.ObservedInput = previousOperationInfo->ObservedInput;
					activeOperationInfo.ObservedOutput = previousOperationInfo->ObservedOutput;
				}
			}

			return generateResult;
		}

		BuildGenerateResult RunEvaluate(
			std::string_view activeCompiler,
			std::string_view flavor,
			std::string_view system,
			std::string_view architecture,
			const Path& packageRoot,
			ValueTable& activeState,
			const std::vector<Path>& buildExtensionLibraries)
		{
			// Ensure the external build extension libraries outlive all usage in the build system
			auto activeExtensionLibraries = std::vector<System::WindowsLibrary>();

			// Create a new build system for the requested build
			auto buildState = BuildState(activeState);
			auto buildSystem = BuildSystem();
			auto activeStateWrapper = Utilities::ValueTableWrapper(buildState.GetActiveState());

			auto outputDirectory = GetOutputDirectory(
				activeCompiler,
				flavor,
				system,
				architecture);
			auto binaryDirectory = GetBinaryDirectory(
				activeCompiler,
				flavor,
				system,
				architecture);
			auto objectDirectory = GetObjectDirectory(
				activeCompiler,
				flavor,
				system,
				architecture);

			// Set the input properties
			activeStateWrapper.EnsureValue("PackageRoot").SetValueString(packageRoot.ToString());
			activeStateWrapper.EnsureValue("BuildFlavor").SetValueString(flavor);
			activeStateWrapper.EnsureValue("BuildSystem").SetValueString(system);
			activeStateWrapper.EnsureValue("BuildArchitecture").SetValueString(architecture);
			activeStateWrapper.EnsureValue("CompilerName").SetValueString(activeCompiler);
			activeStateWrapper.EnsureValue("OutputDirectory").SetValueString(outputDirectory.ToString());
			activeStateWrapper.EnsureValue("BinaryDirectory").SetValueString(binaryDirectory.ToString());
			activeStateWrapper.EnsureValue("ObjectDirectory").SetValueString(objectDirectory.ToString());

			// Run all build extensions
			// Note: Keep the extension libraries open while running the build system
			// to ensure their memory is kept alive
			for (auto buildExtension : buildExtensionLibraries)
			{
				auto library = RunBuildExtension(buildExtension, buildSystem);
				activeExtensionLibraries.push_back(std::move(library));
			}

			// Run the build
			buildSystem.Execute(buildState);

			// Find the output object directory so we can use it in the runner
			auto buildTable = activeStateWrapper.GetValue("Build").AsTable();

			// Convert the generated build into the execution build graph
			auto buildOperations = Utilities::BuildOperationListWrapper(buildState.GetBuildOperations());
			auto operationGraphGenerator = OperationGraphGenerator(_fileSystemState);
			auto graph = operationGraphGenerator.CreateFromDefinition(buildOperations);

			// Return only the build state that is to be passed to the downstream builds
			// This allows the extension dlls to be released and the operations deleted
			auto sharedState = buildState.RetrieveSharedState();

			return BuildGenerateResult(std::move(graph), std::move(sharedState));
		}

		System::WindowsLibrary RunBuildExtension(
			Path& libraryPath,
			IBuildSystem& buildSystem)
		{
			try
			{
				Log::Diag("Running Build Extension: " + libraryPath.ToString());
				auto library = System::WindowsDynamicLibraryManager::LoadDynamicLibrary(
					libraryPath.ToString().c_str());
				auto function = (int(*)(IBuildSystem&))library.GetFunction(
					"RegisterBuildExtension");
				auto result = function(buildSystem);
				if (result != 0)
				{
					Log::Error("Build Extension Failed: " + std::to_string(result));
				}
				else
				{
					Log::Info("Build Extension Done");
				}

				// Keep the library open to ensure the registered tasks are not lost
				return library;
			}
			catch (...)
			{
				Log::Error("Build Extension Failed!");
				throw;
			}
		}

	private:
		FileSystemState& _fileSystemState;
	};
}
