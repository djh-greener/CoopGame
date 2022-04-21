// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/Buff.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABuff::ABuff()
{
	PrimaryActorTick.bCanEverTick = false;
	TotalTicks=1;
	TickInterval=0.f;
	TickProcessed=0;

	BuffState=EBuffState::Active;
	SetReplicates(true);
}

void ABuff::ActivateBuff()
{
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ABuff::TickCounter,TickInterval,true,0);
	BuffState=EBuffState::Stable;//客户端收到OnRep调用
	OnRep_ChangeBuffState();//服务器补充，使两者逻辑相同
}

void ABuff::OnRep_ChangeBuffState()
{
	OnBuffStateChanged(BuffState);
}

void ABuff::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuff,BuffState);
}

void ABuff::TickCounter()
{
	if (TickProcessed>=TotalTicks)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		BuffState=EBuffState::Destroy;//客户端收到OnRep调用
		OnRep_ChangeBuffState();//服务器补充，使两者逻辑相同
		return;
	}
	TickBuff();
	
	TickProcessed++;
}



