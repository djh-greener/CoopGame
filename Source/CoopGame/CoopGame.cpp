// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoopGame.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, CoopGame, "CoopGame" );
TSharedPtr<DDRecord> DDRecord::RecordInst=nullptr;

void DDRecord::InitParams(float showTime)
{
	ShowTime=showTime;
}

TSharedPtr<DDRecord> DDRecord::Get()
{
	if(!RecordInst.IsValid())
	{
		RecordInst=MakeShareable(new DDRecord());
	}
	return RecordInst;
}