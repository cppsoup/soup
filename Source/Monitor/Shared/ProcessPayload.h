#pragma once

namespace Monitor
{
	export struct ProcessPayload
	{
		DWORD nParentProcessId;
		DWORD nTraceProcessId;
		DWORD nGeneology;
		DWORD rGeneology[64];
		WCHAR wzParents[256];
	};

	// Shared payload guid used to get/set the payload when creating the child process
	export constexpr GUID ProcessPayloadResourceId = { 0xd8e2dc69, 0x3004, 0x453e, { 0x94, 0x15, 0x19, 0x0e, 0x79, 0xe8, 0x93, 0x52 } };
}