﻿// <copyright file="SoupApi.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "Models/PackageResultModel.h"
#include "ApiException.h"

namespace Soup::Api
{
    /// <summary>
    /// Represents a collection of functions to interact with the API endpoints
    /// </summary>
    class SoupApi
    {
    public:
        /// <summary>
        /// Download a package version as an archive
        /// </summary>
        // Task<Stream> DownloadPackageAsync(string name, SemanticVersion version)
        // {
        //     using (HttpClient client = new HttpClient())
        //     {
        //         var url = $"{Constants.SoupRESTEndpointV1}/packages/{name}/{version}/{name}_{version}.tgz";
        //         Log.Verbose(url);
        //         var response = await client.GetAsync(url);

        //         // Verify that we got a success
        //         response.EnsureSuccessStatusCode();

        //         // Parse the return result
        //         var result = await response.Content.ReadAsStreamAsync();

        //         return result;
        //     }
        // }

        /// <summary>
        /// Get the metadata for a package identified by the unique name
        /// </summary>
        PackageResultModel GetPackage(std::string_view name)
        {
            auto client = Network::INetworkManager::Current().CreateClient(
                "localhost",
                7071);

            auto urlBuilder = std::stringstream();
            urlBuilder << "/packages/" << name;
            auto url = urlBuilder.str();

            auto response = client->Get(url);

            // Verify that we got a success
            if (response.StatusCode != Network::HttpStatusCode::Ok)
            {
                throw std::runtime_error("GetPackage: Api request failed.");
            }

            // Parse the return result
            // var result = JsonConvert.DeserializeObject<PackageResultModel>(content);
            auto result = PackageResultModel(std::string(name));

            return result;
        }

        /// <summary>
        /// Get a package publication metadata
        /// </summary>
        // Task<PublicationResultModel> GetPublicationAsync(string name, SemanticVersion version)
        // {
        //     using (HttpClient client = new HttpClient())
        //     {
        //         var url = $"{Constants.SoupRESTEndpointV1}/packages/{name}/{version}";
        //         Log.Verbose(url);
        //         var response = await client.GetAsync(url);

        //         // Verify that we got a success
        //         response.EnsureSuccessStatusCode();

        //         // Parse the return result
        //         var content = await response.Content.ReadAsStringAsync();
        //         var result = JsonConvert.DeserializeObject<PublicationResultModel>(content);

        //         return result;
        //     }
        // }

        /// <summary>
        /// Publish a new package version as an archive
        /// </summary>
        // Task<bool> PublishPackageAsync(string name, Stream value)
        // {
        //     using (HttpClient client = new HttpClient())
        //     {
        //         var content = new StreamContent(value);
        //         content.Headers.ContentType = new MediaTypeHeaderValue("application/zip");
        //         var url = $"{Constants.SoupRESTEndpointV1}/packages/{name}";
        //         Log.Verbose(url);
        //         var response = await client.PutAsync(url, content);

        //         // Check if the publish was a no-op
        //         if (response.StatusCode == HttpStatusCode.Conflict)
        //         {
        //             return false;
        //         }

        //         // Verify that we got a success
        //         response.EnsureSuccessStatusCode();

        //         return true;
        //     }
        // }

        /// <summary>
        /// Search for a package using the query string
        /// </summary>
        // Task<SearchModel> SearchPackagesAsync(string q)
        // {
        //     using (HttpClient client = new HttpClient())
        //     {
        //         var url = $"{Constants.SoupRESTEndpointV1}/packages?q={q}";
        //         Log.Verbose(url);
        //         var response = await client.GetAsync(url);

        //         // Verify that we got a success
        //         response.EnsureSuccessStatusCode();

        //         // Parse the return result
        //         var content = await response.Content.ReadAsStringAsync();
        //         var result = JsonConvert.DeserializeObject<SearchModel>(content);

        //         return result;
        //     }
        // }
    };
}
