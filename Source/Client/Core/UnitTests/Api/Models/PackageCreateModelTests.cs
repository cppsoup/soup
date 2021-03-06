// <copyright file="PackageCreateModelTests.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Soup.Api.UnitTests
{
    using Xunit;

    public class PackageCreateModelTests
    {
        [Fact]
        public void DefaultValues()
        {
            var uut = new PackageCreateModel();

            Assert.Null(uut.Name);
            Assert.Null(uut.Description);
        }

        [Fact]
        public void InitializeValues()
        {
            string name = "TestName";
            string description = "TestDescription";

            var uut = new PackageCreateModel()
            {
                Name = name,
                Description = description,
            };

            Assert.Equal(name, uut.Name);
            Assert.Equal(description, uut.Description);
        }

        [Fact]
        public void Serialize_Json()
        {
            string name = "TestName";
            string description = "Test Description";

            var uut = new PackageCreateModel()
            {
                Name = name,
                Description = description,
            };

            var json = TestUtils.JsonSerialize(uut);

            var expected =
                "{\"name\":\"TestName\"," +
                "\"description\":\"Test Description\"}";

            Assert.Equal(expected, json);
        }

        [Fact]
        public void Deserialize_Json()
        {
            string name = "TestName";
            string description = "Test Description";

            var json =
                "{\"name\":\"TestName\"," +
                "\"description\":\"Test Description\"}";

            var value = TestUtils.JsonDeserialize<PackageCreateModel>(json);

            Assert.Equal(name, value.Name);
            Assert.Equal(description, value.Description);
        }
    }
}
