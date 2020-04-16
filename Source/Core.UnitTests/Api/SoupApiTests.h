// <copyright file="SoupApiTests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup::Api::UnitTests
{
	class SoupApiTests
	{
	public:
		[[Fact]]
		void GetPackage_NotFound()
		{
			// Register the network listener
			auto testNetworkManager = std::make_shared<Network::MockNetworkManager>();
			auto scopedNetworkManager = Network::ScopedNetworkManagerRegister(testNetworkManager);

			// Create the required http client
			auto testHttpClient = std::make_shared<Network::MockHttpClient>(
				"api.soupbuild.com",
				80);
			testNetworkManager->RegisterClient(testHttpClient);

			auto packageName = "MissingPackage";
			Assert::ThrowsRuntimeError([&packageName]() {
				auto result = SoupApi::GetPackage(packageName);
			});

			// Verify expected network manager requests
			Assert::AreEqual(
				std::vector<std::string>({
					"CreateClient: api.soupbuild.com:443",
				}),
				testNetworkManager->GetRequests(),
				"Verify network manager requests match expected.");

			// Verify expected http requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Get: /api/v1/packages/MissingPackage",
				}),
				testHttpClient->GetRequests(),
				"Verify http requests match expected.");
		}

		[[Fact]]
		void GetPackage_Success()
		{
			// Register the test listener
			auto testNetworkManager = std::make_shared<Network::MockNetworkManager>();
			auto scopedNetworkManager = Network::ScopedNetworkManagerRegister(testNetworkManager);

			// Create the required http client
			auto testHttpClient = std::make_shared<Network::MockHttpClient>(
				"api.soupbuild.com",
				80);
			testNetworkManager->RegisterClient(testHttpClient);

			// Setup the expected http requests
			auto packageResult = std::string(
				R"({
					"name": "MyPackage"
				})");
			testHttpClient->SetResponse("/api/v1/packages/MyPackage", packageResult);

			auto packageName = "MyPackage";
			auto result = SoupApi::GetPackage(packageName);

			Assert::AreEqual(
				std::vector<std::string>({
					"CreateClient: api.soupbuild.com:443",
				}),
				testNetworkManager->GetRequests(),
				"Verify network manager requests match expected.");

			Assert::AreEqual(
				std::vector<std::string>({
					"Get: /api/v1/packages/MyPackage",
				}),
				testHttpClient->GetRequests(),
				"Verify http requests match expected.");

			Assert::AreEqual(
				PackageResultModel("MyPackage"),
				result,
				"Verify final package matches expected");
		}
	};
}
