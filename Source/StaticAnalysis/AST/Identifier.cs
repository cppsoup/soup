﻿

namespace Soup.StaticAnalysis.AST
{
    /// <summary>
    /// An identifier
    /// </summary>
    public class Identifier : CompoundStatement
    {
        /// <summary>
        /// Gets or sets the value
        /// </summary>
        public string Value { get; set; }
    }
}
