//----------------------------------------------------------------------------
//Chiika
//
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
#include "Win32MediaDetect.h"
#include "MediaDetect.h"
#include "MSAAWin32.h"

#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>

#pragma comment( lib,"Oleacc.lib")

#define NT_SUCCESS(x) ((x) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004

#define ObjectBasicInformation    0
#define ObjectNameInformation     1
#define ObjectTypeInformation     2
#define SystemHandleInformation   16
#define SystemHandleInformationEx 64


#include <string>

#pragma comment(lib, "psapi.lib") 

namespace DisgustingWin32Stuff
{
#pragma region Process-File stuff
	typedef NTSTATUS(NTAPI *_NtQuerySystemInformation)(
		ULONG SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
		);
	typedef NTSTATUS(NTAPI *_NtDuplicateObject)(
		HANDLE SourceProcessHandle,
		HANDLE SourceHandle,
		HANDLE TargetProcessHandle,
		PHANDLE TargetHandle,
		ACCESS_MASK DesiredAccess,
		ULONG Attributes,
		ULONG Options
		);
	typedef NTSTATUS(NTAPI *_NtQueryObject)(
		HANDLE ObjectHandle,
		ULONG ObjectInformationClass,
		PVOID ObjectInformation,
		ULONG ObjectInformationLength,
		PULONG ReturnLength
		);

	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING,*PUNICODE_STRING;

	typedef struct _SYSTEM_HANDLE {
		ULONG ProcessId;
		BYTE ObjectTypeNumber;
		BYTE Flags;
		USHORT Handle;
		PVOID Object;
		ACCESS_MASK GrantedAccess;
	} SYSTEM_HANDLE,*PSYSTEM_HANDLE;
	typedef struct _SYSTEM_HANDLE_INFORMATION {
		ULONG HandleCount;
		SYSTEM_HANDLE Handles[1];
	} SYSTEM_HANDLE_INFORMATION,*PSYSTEM_HANDLE_INFORMATION;

	typedef struct _SYSTEM_HANDLE_EX {
		PVOID Object;
		HANDLE ProcessId;
		HANDLE Handle;
		ULONG GrantedAccess;
		USHORT CreatorBackTraceIndex;
		USHORT ObjectTypeIndex;
		ULONG HandleAttributes;
		ULONG Reserved;
	} SYSTEM_HANDLE_EX,*PSYSTEM_HANDLE_EX;
	typedef struct _SYSTEM_HANDLE_INFORMATION_EX {
		ULONG_PTR HandleCount;
		ULONG_PTR Reserved;
		SYSTEM_HANDLE_EX Handles[1];
	} SYSTEM_HANDLE_INFORMATION_EX,*PSYSTEM_HANDLE_INFORMATION_EX;

	typedef enum _POOL_TYPE {
		NonPagedPool,
		PagedPool,
		NonPagedPoolMustSucceed,
		DontUseThisType,
		NonPagedPoolCacheAligned,
		PagedPoolCacheAligned,
		NonPagedPoolCacheAlignedMustS
	} POOL_TYPE,*PPOOL_TYPE;

	typedef struct _OBJECT_TYPE_INFORMATION {
		UNICODE_STRING Name;
		ULONG TotalNumberOfObjects;
		ULONG TotalNumberOfHandles;
		ULONG TotalPagedPoolUsage;
		ULONG TotalNonPagedPoolUsage;
		ULONG TotalNamePoolUsage;
		ULONG TotalHandleTableUsage;
		ULONG HighWaterNumberOfObjects;
		ULONG HighWaterNumberOfHandles;
		ULONG HighWaterPagedPoolUsage;
		ULONG HighWaterNonPagedPoolUsage;
		ULONG HighWaterNamePoolUsage;
		ULONG HighWaterHandleTableUsage;
		ULONG InvalidAttributes;
		GENERIC_MAPPING GenericMapping;
		ULONG ValidAccess;
		BOOLEAN SecurityRequired;
		BOOLEAN MaintainHandleCount;
		USHORT MaintainTypeList;
		POOL_TYPE PoolType;
		ULONG PagedPoolUsage;
		ULONG NonPagedPoolUsage;
	} OBJECT_TYPE_INFORMATION,*POBJECT_TYPE_INFORMATION;

	PVOID GetLibraryProcAddress(PSTR dll_module,PSTR proc_name) {
		return GetProcAddress(GetModuleHandleA(dll_module),proc_name);
	}

	////////////////////////////////////////////////////////////////////////////////

	static PSYSTEM_HANDLE_INFORMATION_EX GetSystemHandleInformation() {
		auto NtQuerySystemInformation = reinterpret_cast<_NtQuerySystemInformation>(
			GetLibraryProcAddress("ntdll.dll","NtQuerySystemInformation"));

		PSYSTEM_HANDLE_INFORMATION_EX handleInfo = nullptr;

		ULONG handleInfoSize = 0x10000;
		NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;

		do {
			handleInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION_EX>(handleInfo ?
				realloc(handleInfo,handleInfoSize) : malloc(handleInfoSize));
			ZeroMemory(handleInfo,handleInfoSize);

			ULONG requiredSize = 0;
			status = NtQuerySystemInformation(
				SystemHandleInformationEx,handleInfo,handleInfoSize,&requiredSize);

			if(status == STATUS_INFO_LENGTH_MISMATCH) {
				if(requiredSize > handleInfoSize) {
					handleInfoSize = requiredSize;
				}
				else {
					handleInfoSize *= 2;
				}
			}
		} while(status == STATUS_INFO_LENGTH_MISMATCH &&
			handleInfoSize <= 16 * 0x100000);

		if(!NT_SUCCESS(status)) {
			free(handleInfo);
			return nullptr;
		}

		return handleInfo;
	}

	BOOL GetProcessFiles(ULONG process_id,
		std::vector<std::wstring>& files_vector) {
		auto NtDuplicateObject = reinterpret_cast<_NtDuplicateObject>(
			GetLibraryProcAddress("ntdll.dll","NtDuplicateObject"));
		auto NtQueryObject = reinterpret_cast<_NtQueryObject>(
			GetLibraryProcAddress("ntdll.dll","NtQueryObject"));

		HANDLE processHandle = OpenProcess(PROCESS_DUP_HANDLE,FALSE,process_id);
		if(!processHandle)
			return FALSE;

		auto handleInfo = GetSystemHandleInformation();
		if(!handleInfo) {
			CloseHandle(processHandle);
			return FALSE;
		}

		// Type index for files varies between OS versions
		static unsigned short objectTypeFile = 0;

		for(ULONG_PTR i = 0; i < handleInfo->HandleCount; i++) {
			SYSTEM_HANDLE_EX handle = handleInfo->Handles[i];
			NTSTATUS status;

			// Check if this handle belongs to the PID the user specified
			if(reinterpret_cast<ULONG>(handle.ProcessId) != process_id)
				continue;
			// Skip if the handle does not belong to a file
			if(objectTypeFile > 0 && handle.ObjectTypeIndex != objectTypeFile)
				continue;
			// Skip access codes that can cause NtDuplicateObject() or NtQueryObject()
			// to hang
			if(handle.GrantedAccess == 0x00100000 ||
				handle.GrantedAccess == 0x0012008d ||
				handle.GrantedAccess == 0x00120189 ||
				handle.GrantedAccess == 0x0012019f ||
				handle.GrantedAccess == 0x001a019f)
				continue;

			// Duplicate the handle so we can query it
			HANDLE dupHandle = nullptr;
			status = NtDuplicateObject(processHandle,handle.Handle,
				GetCurrentProcess(),&dupHandle,0,0,0);
			if(!NT_SUCCESS(status))
				continue;
			// Query the object type
			POBJECT_TYPE_INFORMATION objectTypeInfo =
				reinterpret_cast<POBJECT_TYPE_INFORMATION>(malloc(0x1000));
			status = NtQueryObject(dupHandle,ObjectTypeInformation,objectTypeInfo,
				0x1000,nullptr);
			if(!NT_SUCCESS(status)) {
				free(objectTypeInfo);
				CloseHandle(dupHandle);
				continue;
			}
			// Determine the type index for files
			if(objectTypeFile == 0) {
				std::wstring type_name(objectTypeInfo->Name.Buffer,
					objectTypeInfo->Name.Length / 2);
				if((type_name == L"File")) {
					objectTypeFile = handle.ObjectTypeIndex;

				}
				else {
					free(objectTypeInfo);
					CloseHandle(dupHandle);
					continue;
				}
			}

			//base::ErrorMode error_mode(SEM_FAILCRITICALERRORS);

			ULONG returnLength;
			PVOID objectNameInfo = malloc(0x1000);
			status = NtQueryObject(dupHandle,ObjectNameInformation,
				objectNameInfo,0x1000,&returnLength);
			if(!NT_SUCCESS(status)) {
				if(returnLength <= 0x10000) {
					// Reallocate the buffer and try again
					objectNameInfo = realloc(objectNameInfo,returnLength);
					status = NtQueryObject(dupHandle,ObjectNameInformation,
						objectNameInfo,returnLength,nullptr);
				}
				if(!NT_SUCCESS(status)) {
					free(objectTypeInfo);
					free(objectNameInfo);
					CloseHandle(dupHandle);
					continue;
				}
			}

			DWORD errorCode = GetLastError();
			if(errorCode != ERROR_SUCCESS)
				SetLastError(ERROR_SUCCESS);

			// Cast our buffer into a UNICODE_STRING
			UNICODE_STRING objectName = *reinterpret_cast<PUNICODE_STRING>(objectNameInfo);
			// Add file path to our list
			if(objectName.Length) {
				std::wstring object_name(objectName.Buffer,objectName.Length / 2);
				files_vector.push_back(object_name);
			}

			free(objectTypeInfo);
			free(objectNameInfo);
			CloseHandle(dupHandle);
		}

		free(handleInfo);
		CloseHandle(processHandle);
		return TRUE;
	}

	typedef struct _ENUM_WND_INFO{
		DWORD dwProcID;
		std::vector<MediaDetect::WndInfo> Handles;
	}ENUM_WND_INFO;


	BOOL CALLBACK EnumWindowsProc(
		__in  HWND hWnd,
		__in  LPARAM lParam
		)
	{
		ENUM_WND_INFO* pEPI = (ENUM_WND_INFO*)lParam;

		DWORD dwProcID = 0;
		GetWindowThreadProcessId(hWnd,&dwProcID);

		int length = ::GetWindowTextLength(hWnd);
		if(0 == length) return TRUE;

		TCHAR windowTitle[MAX_PATH];
		GetWindowText(hWnd,windowTitle,256);


		TCHAR className[MAX_PATH];
		GetClassName(hWnd,className,256);

		MediaDetect::WndInfo wnd;
		wnd.ClassName = className;
		wnd.Title = windowTitle;
		wnd.Handle = (intptr_t)hWnd;
		wnd.ProcessID = dwProcID;

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE,dwProcID);

		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		if(hProcess != NULL)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if(EnumProcessModules(hProcess,&hMod,sizeof(hMod),
				&cbNeeded))
			{
				GetModuleBaseName(hProcess,hMod,szProcessName,
					sizeof(szProcessName)/sizeof(TCHAR));
			}
		}

		CloseHandle( hProcess );

		std::string processName = std::string(szProcessName);
		size_t exeIndex = processName.find_last_of(".exe");

		if(exeIndex != std::string::npos)
			processName = processName.substr(0, processName.size() - 4);
		wnd.ProcessName = processName;
		pEPI->Handles.push_back((wnd));

		return TRUE;
	}
#pragma endregion

#pragma region HDD stuff

	std::vector<std::string> GetDriveLettersFromVolume(
		__in LPCSTR VolumeName
		)
	{
		DWORD  CharCount = MAX_PATH + 1;
		PTCHAR Names     = NULL;
		PTCHAR NameIdx   = NULL;
		BOOL   Success   = FALSE;

		std::vector<std::string> driveLetters;

		for(;;)
		{
			//
			//  Allocate a buffer to hold the paths.
			Names = (PTCHAR) new BYTE[CharCount * sizeof(TCHAR)];

			if(!Names)
			{
				//
				//  If memory can't be allocated, return.
				return driveLetters;
			}

			//
			//  Obtain all of the paths
			//  for this volume.
			Success = GetVolumePathNamesForVolumeNameA(
				VolumeName,Names,CharCount,&CharCount
				);

			if(Success)
			{
				break;
			}

			DWORD lastError = GetLastError();
			if(lastError != ERROR_MORE_DATA)
			{
				break;
			}

			//
			//  Try again with the
			//  new suggested size.
			delete[] Names;
			Names = NULL;
		}

		if(Success)
		{
			//
			//  Display the various paths.
			for(NameIdx = Names;
				NameIdx[0] != L'\0';
				NameIdx += strlen(NameIdx) + 1)
			{
				driveLetters.push_back(NameIdx);
			}
		}

		if(Names != NULL)
		{
			delete[] Names;
			Names = NULL;
		}

		return driveLetters;
	}

	std::map<std::string,std::string> EnumareteVolumes()
	{
		DWORD  CharCount            = 0;
		TCHAR  DeviceName[MAX_PATH] = "";
		DWORD  Error                = ERROR_SUCCESS;
		HANDLE FindHandle           = INVALID_HANDLE_VALUE;
		BOOL   Found                = FALSE;
		size_t Index                = 0;
		BOOL   Success              = FALSE;
		TCHAR  VolumeName[MAX_PATH] = "";

		//
		//  Enumerate all volumes in the system.
		FindHandle = FindFirstVolume(VolumeName,ARRAYSIZE(VolumeName));

		std::map<std::string,std::string> volumeDeviceMap;

		for(;;)
		{
			//
			//  Skip the \\?\ prefix and remove the trailing backslash.
			Index = strlen(VolumeName) - 1;

			if(VolumeName[0]     != L'\\' ||
				VolumeName[1]     != L'\\' ||
				VolumeName[2]     != L'?'  ||
				VolumeName[3]     != L'\\' ||
				VolumeName[Index] != L'\\')
			{
				Error = ERROR_BAD_PATHNAME;
				//wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n",VolumeName);
				break;
			}

			//
			//  QueryDosDeviceW does not allow a trailing backslash,
			//  so temporarily remove it.
			VolumeName[Index] = L'\0';

			CharCount = QueryDosDevice(&VolumeName[4],DeviceName,ARRAYSIZE(DeviceName));

			VolumeName[Index] = L'\\';

			if(CharCount == 0)
			{
				Error = GetLastError();
				wprintf(L"QueryDosDeviceW failed with error code %d\n",Error);
				break;
			}

			volumeDeviceMap.insert(std::make_pair(VolumeName,DeviceName));

			//
			//  Move on to the next volume.
			Success = FindNextVolume(FindHandle,VolumeName,ARRAYSIZE(VolumeName));

			if(!Success)
			{
				Error = GetLastError();

				if(Error != ERROR_NO_MORE_FILES)
				{
					break;
				}

				//
				//  Finished iterating
				//  through all the volumes.
				Error = ERROR_SUCCESS;
				break;
			}
		}

		FindVolumeClose(FindHandle);
		FindHandle = INVALID_HANDLE_VALUE;

		return volumeDeviceMap;
	}
#pragma endregion

#pragma region MSAA STUFF (Adapted from https://github.com/erengy/taiga)

	AccessibleChild::AccessibleChild()
		: role(0) {
	}

	AccessibleObject::AccessibleObject()
		: acc_(nullptr),
		win_event_hook_(nullptr) {
	}

	AccessibleObject::~AccessibleObject() {
		Release();
	}

	HRESULT AccessibleObject::FromWindow(HWND hwnd,DWORD object_id) {
		if(hwnd == nullptr)
			return E_INVALIDARG;

		Release();

		return AccessibleObjectFromWindow(hwnd,object_id,IID_IAccessible,
			(void**)&acc_);
	}

	void AccessibleObject::Release() {
		if(acc_ != nullptr) {
			acc_->Release();
			acc_ = nullptr;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	HRESULT AccessibleObject::GetName(std::string& name,long child_id,
		IAccessible* acc) {
		if(acc == nullptr)
			acc = acc_;
		if(acc == nullptr)
			return E_INVALIDARG;

		VARIANT var_child;
		var_child.vt = VT_I4;
		var_child.lVal = child_id;

		BSTR buffer;
		HRESULT hr = acc->get_accName(var_child,&buffer);

		if(buffer) {
			std::wstring wstr = std::wstring(buffer);
			name = std::string(wstr.begin(),wstr.end());
		}
		else {
			name.clear();
		}
		SysFreeString(buffer);

		return hr;
	}

	HRESULT AccessibleObject::GetRole(DWORD& role,long child_id,
		IAccessible* acc) {
		if(acc == nullptr)
			acc = acc_;
		if(acc == nullptr)
			return E_INVALIDARG;

		VARIANT var_child;
		var_child.vt = VT_I4;
		var_child.lVal = child_id;

		VARIANT var_result;
		HRESULT hr = acc->get_accRole(var_child,&var_result);

		if(hr == S_OK && var_result.vt == VT_I4) {
			role = var_result.lVal;
			return S_OK;
		}

		return E_FAIL;
	}

	HRESULT AccessibleObject::GetRole(std::string& role,long child_id,
		IAccessible* acc) {
		DWORD role_id = 0;

		HRESULT hr = GetRole(role_id,child_id,acc);

		if(hr != S_OK)
			return hr;

		UINT role_length = GetRoleText(role_id,nullptr,0);
		LPTSTR buffer = (LPTSTR)malloc((role_length + 1) * sizeof(TCHAR));

		if(buffer != nullptr) {
			GetRoleText(role_id,buffer,role_length + 1);
			if(buffer) {
				role = buffer;
			}
			else {
				role.clear();
			}
			free(buffer);
		}
		else {
			return E_OUTOFMEMORY;
		}

		return S_OK;
	}

	HRESULT AccessibleObject::GetValue(std::string& value,long child_id,
		IAccessible* acc) {
		if(acc == nullptr)
			acc = acc_;
		if(acc == nullptr)
			return E_INVALIDARG;

		VARIANT var_child;
		var_child.vt = VT_I4;
		var_child.lVal = child_id;

		BSTR buffer;
		HRESULT hr = acc->get_accValue(var_child,&buffer);

		if(buffer) {
			std::wstring wstr = std::wstring(buffer);
			value = std::string(wstr.begin(),wstr.end());
		}
		else {
			value.clear();
		}
		SysFreeString(buffer);

		return hr;
	}

	////////////////////////////////////////////////////////////////////////////////

	HRESULT AccessibleObject::GetChildCount(long* child_count,IAccessible* acc) {
		if(acc == nullptr)
			acc = acc_;
		if(acc == nullptr)
			return E_INVALIDARG;

		return acc->get_accChildCount(child_count);
	}

	HRESULT AccessibleObject::BuildChildren(std::vector<AccessibleChild>& children,
		IAccessible* acc,LPARAM param) {
		if(acc == nullptr)
			acc = acc_;
		if(acc == nullptr)
			return E_INVALIDARG;

		long child_count = 0;
		HRESULT hr = acc->get_accChildCount(&child_count);

		if(FAILED(hr))
			return hr;
		if(child_count == 0)
			return S_FALSE;

		long obtained_count = 0;
		std::vector<VARIANT> var_array(child_count);
		hr = AccessibleChildren(acc,0L,child_count,var_array.data(),
			&obtained_count);

		if(FAILED(hr))
			return hr;

		children.resize(obtained_count);
		for(int i = 0; i < obtained_count; i++) {
			VARIANT var_child = var_array[i];

			if(var_child.vt == VT_DISPATCH) {
				IDispatch* dispatch = var_child.pdispVal;
				IAccessible* child = nullptr;
				hr = dispatch->QueryInterface(IID_IAccessible,(void**)&child);
				if(hr == S_OK) {
					GetName(children.at(i).name,CHILDID_SELF,child);
					GetRole(children.at(i).role,CHILDID_SELF,child);
					GetValue(children.at(i).value,CHILDID_SELF,child);
					if(AllowChildTraverse(children.at(i),param))
						BuildChildren(children.at(i).children,child,param);
					child->Release();
				}
				dispatch->Release();

			}
			else {
				GetName(children.at(i).name,var_child.lVal,acc);
				GetRole(children.at(i).role,var_child.lVal,acc);
				GetValue(children.at(i).value,var_child.lVal,acc);
			}
		}

		return S_OK;
	}

	bool AccessibleObject::AllowChildTraverse(AccessibleChild& child,
		LPARAM param) {
		switch(param) {
		case -1:
			return false;

		case 0: //Chrome
			switch(child.role) {
			case ROLE_SYSTEM_CLIENT:
			case ROLE_SYSTEM_GROUPING:
			case ROLE_SYSTEM_PAGETABLIST:
			case ROLE_SYSTEM_TEXT:
			case ROLE_SYSTEM_TOOLBAR:
			case ROLE_SYSTEM_WINDOW:
				return true;
			default:
				return false;
			}
			break;

		case 1: //Firefox
			switch(child.role) {
			case ROLE_SYSTEM_APPLICATION:
			case ROLE_SYSTEM_COMBOBOX:
			case ROLE_SYSTEM_PAGETABLIST:
			case ROLE_SYSTEM_TOOLBAR:
				return true;
			case ROLE_SYSTEM_DOCUMENT:
			default:
				return false;
			}
			break;

		case 2:
			switch(child.role) {
			case ROLE_SYSTEM_PANE:
			case ROLE_SYSTEM_SCROLLBAR:
				return false;
			}
			break;

		case 3:
			switch(child.role) {
			case ROLE_SYSTEM_DOCUMENT:
			case ROLE_SYSTEM_PANE:
				return false;
			}
			break;
		}

		return true;
	}

	AccessibleChild* FindAccessibleChild(
		std::vector<AccessibleChild>& children,
		const std::string& name,
		DWORD role) {
		AccessibleChild* child = nullptr;

		for(auto it = children.begin();it != children.end(); ++it) {
			if(name.empty() || strcmp(name.c_str(),it->name.c_str()) == 0)
				if(!role || role == it->role)
					child = &(*it);
			if(child == nullptr && !it->children.empty())
				child = FindAccessibleChild(it->children,name,role);
			if(child)
				break;
		}

		return child;
	}
#pragma endregion
}


namespace MediaDetect
{
	Win32MediaDetect::Win32MediaDetect()
	{
		videoFormats =
		{
			".mkv",
			".mp4",
			".avi",
			".wmv",
			".mpeg",
			".mpg",
			".mov"
		};

	}

	Win32MediaDetect::~Win32MediaDetect()
	{

	}

	std::vector<std::string> Win32MediaDetect::GetFilesOpenByProcess(UINT PID)
	{
		std::vector<std::wstring> files;
		std::vector<std::string> videoFiles;
		DisgustingWin32Stuff::GetProcessFiles(PID,files);

		typedef std::map<std::string,std::string> VolumeDeviceMap;
		std::map<std::string,std::string> volumeDeviceMap = DisgustingWin32Stuff::EnumareteVolumes(); // (VolumeName,DeviceName)

		for(VolumeDeviceMap::iterator It = volumeDeviceMap.begin(); It != volumeDeviceMap.end(); ++It)
		{
			std::vector<std::string> driveLetters = DisgustingWin32Stuff::GetDriveLettersFromVolume(It->first.c_str());
		}

		for(int i=0; i < files.size(); ++i)
		{
			std::string fileName = std::string(files[i].begin(),files[i].end());

			size_t index = std::string::npos;

			for(int v = 0; v < videoFormats.size(); ++v) {
				index = fileName.find(videoFormats[v]);
				if(index != std::string::npos)
					break;
			}

			if(index != std::string::npos)
			{
				size_t device = fileName.find("\\",2);
				size_t drive = fileName.find("\\",device + 2);

				std::string driveName = fileName.substr(0,drive);

				for(VolumeDeviceMap::iterator It = volumeDeviceMap.begin(); It != volumeDeviceMap.end(); ++It)
				{
					if(It->second == driveName)
					{
						std::vector<std::string> driveLetters = DisgustingWin32Stuff::GetDriveLettersFromVolume(It->first.c_str());

						fileName.replace(0,drive,driveLetters[0].substr(0,2));
					}
				}
				videoFiles.push_back(fileName);
			}
		}
		return videoFiles;
	}

	std::vector<WndInfo> Win32MediaDetect::GetWindows()
	{
		DisgustingWin32Stuff::ENUM_WND_INFO windows ={0};
		ZeroMemory(&windows,sizeof(DisgustingWin32Stuff::ENUM_WND_INFO));

		bool b = EnumWindows(&DisgustingWin32Stuff::EnumWindowsProc,(LPARAM)&windows);
		return windows.Handles;
	}

	std::string Win32MediaDetect::GetActiveTabLink(intptr_t handle,unsigned browser)
	{
		std::string link;
		switch(browser)
		{
		case 0: //Chrome
		case 1: //Firefox
		case 2: //IE
		{
			DisgustingWin32Stuff::AccessibleObject accObj;
			if(!FAILED(accObj.FromWindow((HWND)handle)))
			{
				accObj.BuildChildren(accObj.children,nullptr,browser);
				accObj.Release();
			}

			DisgustingWin32Stuff::AccessibleChild* accChild = FindAccessibleChild(accObj.children,"",ROLE_SYSTEM_TEXT);

			if(accChild)
			{
				link = accChild->value;
			}
		}
		break;
		default:
			break;
		}
		return link;
	}

	bool Win32MediaDetect::CheckIfTabWithTitleIsOpen(const std::string& title,intptr_t handle,unsigned browser)
	{
		switch(browser)
		{
		case 0: //Chrome
		case 1: //Firefox
		case 2: //IE
		{
			DisgustingWin32Stuff::AccessibleObject accObj;
			if(!FAILED(accObj.FromWindow((HWND)handle)))
			{
				accObj.BuildChildren(accObj.children,nullptr,browser);
				accObj.Release();
			}

			DisgustingWin32Stuff::AccessibleChild* accChild = FindAccessibleChild(accObj.children,"",ROLE_SYSTEM_PAGETABLIST);

			if(accChild)
			{
				for(int i = 0;i < accChild->children.size(); ++i)
				{
					if(strcmp(accChild->children[i].name.c_str(),title.c_str()) == 0)
					{
						return true;
					}
				}
			}
		}
		break;
		default:
			break;
		}
		return false;
	}
}
