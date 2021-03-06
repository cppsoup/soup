﻿// <copyright file="BuildGenerateEngine.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Soup.Build.Runtime;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace Soup.Build.Generate
{
	/// <summary>
	/// The core build generate engine that knows how to run all of the required build Tasks to generate the
	/// Operation Graph.
	/// </summary>
	internal class BuildGenerateEngine
	{
		public BuildGenerateEngine()
		{
			_fileSystemState = new FileSystemState();
		}

		/// <summary>
		/// Execute the entire operation graph that is referenced by this build generate engine
		/// </summary>
		public void Generate(Path soupTargetDirectory)
		{
			// Run all build operations in the correct order with incremental build checks
			Log.Diag("Build generate start");

			// Load the parameters file
			var parametersFile = soupTargetDirectory + BuildConstants.GenerateParametersFileName;
			if (!ValueTableManager.TryLoadState(parametersFile, out var parametersState))
			{
				Log.Error("Failed to load the parameter file: " + parametersFile.ToString());
				throw new InvalidOperationException("Failed to load parameter file.");
			}

			// Get the required input state from the parameters
			var targetDirectory = new Path(parametersState["TargetDirectory"].AsString().ToString());
			var packageDirectory = new Path(parametersState["PackageDirectory"].AsString().ToString());

			// Load the recipe file
			var recipeFile = packageDirectory + BuildConstants.RecipeFileName;
			if (!RecipeExtensions.TryLoadRecipeFromFile(recipeFile, out var recipe))
			{
				Log.Error("Failed to load the recipe: " + recipeFile.ToString());
				throw new InvalidOperationException("Failed to load recipe.");
			}

			// Combine all the dependencies shared state
			var dependenciesSharedState = LoadDependenciesSharedState(parametersState);

			// Generate the set of build extension libraries
			var buildExtensionLibraries = GenerateBuildExtensionSet(recipe, dependenciesSharedState);

			// Start a new active state that is initialized to the recipe itself
			var activeState = new ValueTable();

			// Initialize the Recipe Root Table
			var recipeState = recipe.GetTable();
			activeState.Add("Recipe", new Value(recipeState));

			// Initialize the Parameters Root Table
			activeState.Add("Parameters", new Value(parametersState));

			// Initialize the Dependencies Root Table
			activeState.Add("Dependencies", new Value(dependenciesSharedState));

			// Keep the extension libraries open while running the build system
			// to ensure their memory is kept alive
			var evaluateGraph = new OperationGraph();
			IValueTable sharedState = new ValueTable();

			{
				// Create a new build system for the requested build
				var buildTaskManager = new BuildTaskManager();

				// Run all build extension register callbacks
				foreach (var buildExtension in buildExtensionLibraries)
				{
					var library = LoadPlugin(buildExtension);
					FindAllCommands(library, buildTaskManager);
				}

				// Run the build
				var buildState = new BuildState(activeState, _fileSystemState);
				buildTaskManager.Execute(buildState);

				// Grab the build results so the dependency libraries can be released asap
				evaluateGraph = buildState.BuildOperationGraph();
				sharedState = buildState.SharedState;
			}

			// Save the operation graph so the evaluate phase can load it
			var evaluateGraphFile = soupTargetDirectory + BuildConstants.GenerateEvaluateOperationGraphFileName;
			OperationGraphManager.SaveState(evaluateGraphFile, evaluateGraph, _fileSystemState);

			// Save the shared state that is to be passed to the downstream builds
			var sharedStateFile = soupTargetDirectory + BuildConstants.GenerateSharedStateFileName;
			ValueTableManager.SaveState(sharedStateFile, sharedState);
			Log.Diag("Build generate end");
		}

		/// <summary>
		/// Using the parameters to resolve the dependency output folders, load up the shared state table and
		/// combine them into a single value table to be used as input the this generate phase.
		/// </summary>
		private ValueTable LoadDependenciesSharedState(IValueTable parametersTable)
		{
			var sharedDependenciesTable = new ValueTable();
			if (parametersTable.TryGetValue("Dependencies", out var dependencyTableValue))
			{
				var dependenciesTable = dependencyTableValue.AsTable();
				foreach (var dependencyTypeValue in dependenciesTable)
				{
					var dependencyType = dependencyTypeValue.Key;
					var dependencies = dependencyTypeValue.Value.AsTable();
					foreach (var dependencyValue in dependencies)
					{
						var dependencyName = dependencyValue.Key;
						var dependency = dependencyValue.Value.AsTable();
						var soupTargetDirectory = new Path(dependency["SoupTargetDirectory"].AsString());
						var sharedStateFile = soupTargetDirectory + BuildConstants.GenerateSharedStateFileName;

						// Load the shared state file
						if (!ValueTableManager.TryLoadState(sharedStateFile, out var sharedStateTable))
						{
							Log.Error("Failed to load the shared state file: " + sharedStateFile.ToString());
							throw new InvalidOperationException("Failed to load shared state file.");
						}

						// Add the shared build state from this child build into the correct
						// table depending on the build type
						var typedDependenciesTable = EnsureValueTable(sharedDependenciesTable, dependencyType);
						typedDependenciesTable.Add(
							dependencyName,
							new Value(sharedStateTable));
					}
				}
			}

			return sharedDependenciesTable;
		}

		static void FindAllCommands(
			Assembly assembly,
			BuildTaskManager buildTaskManager)
		{
			foreach (Type type in assembly.GetTypes())
			{
				if (type.IsClass &&
					type.IsPublic &&
					!type.IsAbstract &&
					typeof(IBuildTask).IsAssignableFrom(type))
				{
					var runBeforeListPropertyInfo = type.GetProperty("RunBeforeList", BindingFlags.Public | BindingFlags.Static);
					if (runBeforeListPropertyInfo is null)
						throw new InvalidOperationException("Type missing RunBeforeList");

					var runBeforeList = (IReadOnlyList<string>?)runBeforeListPropertyInfo.GetValue(null, null);
					if (runBeforeList is null)
						throw new InvalidOperationException("RunBeforeList is null");

					var runAfterListPropertyInfo = type.GetProperty("RunAfterList", BindingFlags.Public | BindingFlags.Static);
					if (runAfterListPropertyInfo is null)
						throw new InvalidOperationException("Type missing RunAfterList");

					var runAfterList = (IReadOnlyList<string>?)runAfterListPropertyInfo.GetValue(null, null);
					if (runAfterList is null)
						throw new InvalidOperationException("RunAfterList is null");

					buildTaskManager.RegisterTask(type.Name, type, runBeforeList, runAfterList);
				}
			}
		}

		private IValueTable EnsureValueTable(IValueTable table, string key)
		{
			if (table.ContainsKey(key))
			{
				return table[key].AsTable();
			}
			else
			{
				var value = new ValueTable();
				table.Add(key, new Value(value));
				return value;
			}
		}

		/// <summary>
		/// Generate the collection of build extensions
		/// </summary>
		private IList<Path> GenerateBuildExtensionSet(
			Recipe recipe,
			ValueTable dependenciesSharedState)
		{
			var buildExtensionLibraries = new List<Path>();

			// Run the RecipeBuild extension to inject core build tasks
			var recipeBuildExtensionPath = new Path();
			var language = recipe.Language;
			if (language == "C++")
			{
				var moduleFolder = new Path(Assembly.GetExecutingAssembly().Location).GetParent();
				recipeBuildExtensionPath = moduleFolder + new Path("Extensions/Soup.Cpp/Soup.Cpp.dll");
			}
			else if (language == "C#")
			{
				var moduleFolder = new Path(Assembly.GetExecutingAssembly().Location).GetParent();
				recipeBuildExtensionPath = moduleFolder + new Path("Extensions/Soup.CSharp/Soup.CSharp.dll");
			}
			else
			{
				throw new InvalidOperationException("Unknown language.");
			}

			buildExtensionLibraries.Add(recipeBuildExtensionPath);

			// Check for any dynamic libraries in the shared state
			if (dependenciesSharedState.TryGetValue("Build", out var buildValue))
			{
				foreach (var dependencyValue in buildValue.AsTable())
				{
					var dependency = dependencyValue.Value.AsTable();
					if (dependency.TryGetValue("TargetFile", out var targetFileValue))
					{
						var targetFile = new Path(targetFileValue.AsString().ToString());

						if (System.IO.File.Exists(targetFile.ToString()))
						{
							buildExtensionLibraries.Add(targetFile);
						}
					}
				}
			}

			return buildExtensionLibraries;
		}

		static Assembly LoadPlugin(Path libraryPath)
		{
			// Navigate up to the solution root
			Console.WriteLine($"Loading Plugin Assembly: {libraryPath}");
			var loadContext = new ExtensionLoadContext(libraryPath.ToString());
			return loadContext.LoadFromAssemblyName(new AssemblyName(libraryPath.GetFileStem()));
		}

		private FileSystemState _fileSystemState;
	}
}
