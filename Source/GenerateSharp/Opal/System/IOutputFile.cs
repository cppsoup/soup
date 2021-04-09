﻿// <copyright file="IOutputFile.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.IO;

namespace Opal.System
{
	/// <summary>
	/// The output file interface
	/// Interface mainly used to allow for unit testing client code
	/// </summary>
	public interface IOutputFile : IFile
	{
		/// <summary>
		/// Gets the output stream
		/// </summary>
		Stream GetOutStream();
	}
}
