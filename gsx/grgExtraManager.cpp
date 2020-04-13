#define _CRT_SECURE_NO_WARNINGS
#include "grgExtraManager.h"
#include "CStoredCar.h"
#include <injector\injector.hpp>
#include <injector\assembly.hpp>
#include <injector\calling.hpp>
#include <cstdint>

static injector::auto_pointer fun, funSave;
static CVehicle *vehicle = nullptr;
static CStoredCar *storedCar = nullptr;

grgExtraManager::OnRestoreType &grgExtraManager::OnRestoreCallback()
{
	static OnRestoreType cb; return cb;
}

grgExtraManager::OnRestoreBefType &grgExtraManager::OnRestoreBefCallback()
{
	static OnRestoreBefType cb; return cb;
}

grgExtraManager::OnSaveType &grgExtraManager::OnSaveCallback()
{
	static OnSaveType cb; return cb;
}

static void __declspec(naked) restoreHook()
{
	__asm
	{
		mov vehicle, ecx
		mov storedCar, edi

		pushad
		pushfd

		call grgExtraManager::internalWrapperRestore

		popfd
		popad
		
		jmp fun
	}
}

namespace inject
{
	void* funRet = 0;
	//
	template <uintptr_t V> struct wrapper
	{
		static void* &retaddr()
		{
			static void* addr;
			return addr;
		}
	};

	template <class T>
	static void __declspec(naked) restoreHookBeforeSpawn()
	{
		__asm
		{
			mov vehicle, ecx
			mov storedCar, edi

			pushad
			pushfd

			call grgExtraManager::internalWrapperRestoreBef
			call T::retaddr
			mov eax, [eax]
			mov funRet, eax

			popfd
			popad

			jmp funRet
		}
	}

	template <uintptr_t T>
	static void MakeCALL()
	{
		typedef wrapper<T> E;
		E::retaddr() = injector::MakeCALL(T, restoreHookBeforeSpawn<E>).get();
	}
}

static void __declspec(naked) saveHook()
{
	__asm
	{
		mov vehicle, ecx
			mov storedCar, edi

			pushad
			pushfd

			call grgExtraManager::internalWrapperSave

			popfd
			popad

			jmp funSave
	}
}

void grgExtraManager::internalWrapperRestore()
{
	grg().veh = vehicle;

	auto &restoreFun = OnRestoreCallback();

	if (restoreFun)
	{
		restoreFun(vehicle, storedCar);
	}
}

void grgExtraManager::internalWrapperRestoreBef()
{
	grg().veh = vehicle;

	OnRestoreBefType &restoreFunBef = OnRestoreBefCallback();

	if (restoreFunBef)
	{
		restoreFunBef(vehicle, storedCar);
	}
}

void grgExtraManager::internalWrapperSave()
{
	grg().veh = vehicle;

	auto &saveFun = OnSaveCallback();

	if (saveFun)
	{
		saveFun(vehicle, storedCar);
	}
}

grgExtraManager &grgExtraManager::grg()
{
	static grgExtraManager obj;
	return obj;
}

grgExtraManager::grgExtraManager() : veh(nullptr)
{

	inject::MakeCALL<0x004480D0>();
	inject::MakeCALL<0x00447F2C>();
	inject::MakeCALL<0x00447F60>();
	inject::MakeCALL<0x00447F94>();
	inject::MakeCALL<0x00447FC8>();
	inject::MakeCALL<0x00447FF8>();
	inject::MakeCALL<0x0044802F>();
	inject::MakeCALL<0x00448074>();
	inject::MakeCALL<0x004480A1>();



	fun = injector::MakeCALL(0x0044828D, restoreHook).get();
	//funBef = injector::MakeCALL(0x00447E63, restoreHookBeforeSpawn).get();
	funSave = injector::MakeCALL(0x004498E4, saveHook).get();

}
