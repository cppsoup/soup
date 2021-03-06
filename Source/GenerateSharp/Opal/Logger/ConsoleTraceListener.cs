// <copyright file="ConsoleTraceListener.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;

namespace Opal
{
	/// <summary>
	/// Console logger that wraps the base <see cref="TraceListener"/>
	/// </summary>
	public class ConsoleTraceListener : TraceListener
	{
		/// <summary>
		/// Initializes a new instance of the <see cref='ConsoleTraceListener'/> class.
		/// </summary>
		public ConsoleTraceListener() :
			base()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref='ConsoleTraceListener'/> class.
		/// </summary>
		public ConsoleTraceListener(
			string name,
			IEventFilter filter,
			bool showEventType,
			bool showEventId) :
			base(
				name,
				filter,
				showEventType,
				showEventId)
		{
		}

		/// <summary>
		/// Writes a message and newline terminator
		/// </summary>
		protected override void WriteLine(string message)
		{
			// if (NeedIndent)
			// {
			//	 WriteIndent();
			// }

			SetConsoleColor();
			Console.WriteLine(message);
			// TODO: restore color

			// NeedIndent = true;
		}

		public void SetConsoleColor()
		{
			// TODO
			// switch (_currentEvent)
			// {
			//	 case TraceEventType.Error:
			//		 Console.ForegroundColor = ConsoleColor.Red;
			//		 break;
			//	 case TraceEventType.Warning:
			//		 Console.ForegroundColor = ConsoleColor.Yellow;
			//		 break;
			// }
		}
	}
}
