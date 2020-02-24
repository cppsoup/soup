﻿// <copyright file="IHttpClient.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "HttpResponse.h"

namespace Opal::Network
{
	export using Headers = std::multimap<std::string, std::string>;

	/// <summary>
	/// The Http Client interface
	/// Interface mainly used to allow for unit testing client code
	/// </summary>
	export class IHttpClient
	{
	public:
		/// <summary>
		/// Perform an Http Get request
		/// </summary>
		virtual HttpResponse Get(std::string_view request) = 0;

		/// <summary>
		/// Perform an Http Put request
		/// </summary>
		virtual HttpResponse Put(
			std::string_view request,
			std::string_view contentType,
			std::istream& content) = 0;
	};
}
