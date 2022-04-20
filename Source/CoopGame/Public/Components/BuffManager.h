// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/Buff.h"
#include "Components/ActorComponent.h"
#include "BuffManager.generated.h"

//负责管理buff的生命周期
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API UBuffManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMap<FName,TArray<ABuff*>>BuffTable{
		{"Active",TArray<ABuff*>()},
		{"Stable",TArray<ABuff*>()}
	};
	
};
