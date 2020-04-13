#pragma once
#ifndef GTASA_GRGEXTRAITEMS
#define GTASA_GRGEXTRAITEMS
#include <functional>
#include "CStoredCar.h"

class grgExtraManager
{
	typedef std::function<void(CVehicle*, CStoredCar*)> OnRestoreType;
	typedef std::function<void(CVehicle*, CStoredCar*)> OnRestoreBefType;
	typedef std::function<void(CVehicle*, CStoredCar*)> OnSaveType;

	static OnRestoreType& OnRestoreCallback();
	static OnRestoreBefType& OnRestoreBefCallback();
	static OnSaveType& OnSaveCallback();

	CVehicle *veh;

	static void internalWrapperRestore();
	static void internalWrapperRestoreBef();
	static void internalWrapperSave();

public:
	static void on_restore(const OnRestoreType& fn)
	{
		grg();
		OnRestoreCallback() = fn;
	}

	static void on_restore_bef(const OnRestoreBefType& fn)
	{
		grg();
		OnRestoreBefCallback() = fn;
	}

	static void on_save(const OnSaveType& fn)
	{
		grg();
		OnSaveCallback() = fn;
	}

	CVehicle *getVeh() { return veh; }

	static grgExtraManager &grg();

private:
	grgExtraManager();
};


#endif
