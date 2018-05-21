﻿// <copyright company="Soup" file="GenerateCommand.cs">
//   Copyright (c) Soup.  All rights reserved.
// </copyright>

namespace Soup.Client
{
	using System.IO;
	using System.Threading.Tasks;

	/// <summary>
	/// Generate Command
	/// </summary>
	internal class GenerateCommand : ICommand
	{
		public string Name => "generate";

		public async Task InvokeAsync(string[] args)
		{
			Log.Message("Generate");
			var projectDirectory = Directory.GetCurrentDirectory().EnsureTrailingSlash();
			var buildPath = Path.Combine(
				projectDirectory,
				Constants.ProjectGenerateFolderName,
				Constants.StoreBuildFolderName).EnsureTrailingSlash();
			var recipe = await RecipeManager.LoadFromFileAsync(projectDirectory);
			if (recipe == null)
			{
				Log.Error("Could not find the recipe file.");
				return;
			}

			// Ensure the project folder exists
			PackageManager.EnsureProjectGenerateFolderExists(projectDirectory);
			if (!Directory.Exists(buildPath))
			{
				Directory.CreateDirectory(buildPath);
			}

			// Generate the project files
			var buildEngine = Singleton<IBuildEngine>.Instance;
			await buildEngine.GenerateDependenciesAsync(
				recipe,
				buildPath);
			await buildEngine.GenerateBuildAsync(
				recipe,
				buildPath,
				projectDirectory,
				@"out/bin",
				@"out/obj");
		}
	}
}
