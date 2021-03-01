#include <windows.h>

template <typename T>
class VftableHook {
	
	T* org_vftable;
	T* new_vftable;
	LPVOID pvftable;
	bool is_hook;

	VftableHook(T* t) {
		this->org_vftable = t;
	}
	VftableHook() {

	}
	
	bool Hook() {
		//vftable指针指向正确内存块开始HOOK
		if (this->is_hook){
			return true;
		}
		if (IsBadReadPtr(this->org_vftable, 4096) == 0){
			this->pvftable = new BYTE[4096];
			memcpy(this->pvftable, this->org_vftable, 4096);
			this->is_hook = true;
		}
		
		return true;
	}
	bool Hook(T* pvftable) {
		this->org_vftable = pvftable;
		return Hook();

	}
	bool UnHook() {
		delete this->c_pvftable;
		this->is_hook = false;
		return true;
	}
};
