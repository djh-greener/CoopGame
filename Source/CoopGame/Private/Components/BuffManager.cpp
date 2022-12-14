// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BuffManager.h"
#include "SCharacter.h"

// Sets default values for this component's properties
UBuffManager::UBuffManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//SetIsReplicated(true);
}

// Called when the game starts
void UBuffManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UBuffManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<ABuff*>TmpBuffGroup;
	//检查Stable列表，如果存在待销毁Buff，集中销毁
	TmpBuffGroup.Empty();
	for (auto it:StableBuffs)
		if(it->BuffState==EBuffState::Destroy)
			TmpBuffGroup.Add(it);
	for (auto it:TmpBuffGroup)
	{
		StableBuffs.Remove(it);
		it->Destroy();
	}

	//检查Active列表，如果存在待激活Buff，立即激活并转移至Stable列表
	TmpBuffGroup.Empty();
	for (auto it:ActiveBuffs)
		TmpBuffGroup.Add(it);
	for (auto it:TmpBuffGroup)
	{
		ASCharacter*Player=Cast<ASCharacter>(GetOwner());
		it->ActivateBuff(Player);
		ActiveBuffs.Remove(it);
		StableBuffs.Add(it);
	}
}

