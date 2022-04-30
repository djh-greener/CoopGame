// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/Buff.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABuff::ABuff()
{
	PrimaryActorTick.bCanEverTick = false;
	TotalTicks=1;
	TickInterval=0.f;
	TickProcessed=0;

	BuffState=EBuffState::Active;
	//SetReplicates(true);
}

void ABuff::ActivateBuff(ASCharacter*Player)//此函数只在服务器被调用
{
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ABuff::TickCounter,TickInterval,true,0);
	BuffState=EBuffState::Stable;
	BuffObject=Player;
}

void ABuff::TickCounter()
{
	if (TickProcessed>=TotalTicks)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		BuffState=EBuffState::Destroy;
		return;
	}
	TickBuff(BuffObject);
	
	TickProcessed++;
}



