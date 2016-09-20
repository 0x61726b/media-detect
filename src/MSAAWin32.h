#include <string>
#include <OleAcc.h>
#include <vector>

namespace DisgustingWin32Stuff {


	struct AccessibleChild {
		AccessibleChild();

		std::string name;
		DWORD role;
		std::string value;
		std::vector<AccessibleChild> children;
	};

	class AccessibleObject {
	public:
		AccessibleObject();
		virtual ~AccessibleObject();

		HRESULT FromWindow(HWND hwnd,DWORD object_id = OBJID_CLIENT);
		void Release();

		HRESULT GetName(std::string& name,long child_id = CHILDID_SELF,
			IAccessible* acc = nullptr);
		HRESULT GetRole(DWORD& role,long child_id = CHILDID_SELF,
			IAccessible* acc = nullptr);
		HRESULT GetRole(std::string& role,long child_id = CHILDID_SELF,
			IAccessible* acc = nullptr);
		HRESULT GetValue(std::string& value,long child_id = CHILDID_SELF,
			IAccessible* acc = nullptr);

		HRESULT BuildChildren(std::vector<AccessibleChild>& children,
			IAccessible* acc = nullptr,LPARAM param = 0L);
		HRESULT GetChildCount(long* child_count,IAccessible* acc = nullptr);

		virtual bool AllowChildTraverse(AccessibleChild& child,LPARAM param = 0L);

		std::vector<AccessibleChild> children;

	private:
		IAccessible* acc_;
		HWINEVENTHOOK win_event_hook_;
	};
}