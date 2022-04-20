// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/Buff.h"

// Sets default values
ABuff::ABuff()
{
	PrimaryActorTick.bCanEverTick = false;
	TotalTicks=1;
	TickInterval=0.f;
	TickProcessed=0;

	BuffState=EBuffState::Active;
}

void ABuff::ActivateBuff()
{
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ABuff::TickCounter,TickInterval,true,0);
	BuffState=EBuffState::Stable;
}

void ABuff::TickCounter()
{
	if (TickProcessed>=TotalTicks)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		BuffState=EBuffState::Destroy;
		return;
	}
	TickBuff();
	
	TickProcessed++;
}

void ABuff::BeginPlay()
{
	Super::BeginPlay();
	
}



