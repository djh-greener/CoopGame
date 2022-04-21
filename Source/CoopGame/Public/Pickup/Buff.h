// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buff.generated.h"

UENUM()
enum class EBuffState:uint8
{
	Active=0,
	Stable,
	Destroy
};

UCLASS()
class COOPGAME_API ABuff : public AActor
{
	GENERATED_BODY()
	
public:	
	// 设置为Active需要激活
	ABuff();

	//Manager激活
	void ActivateBuff();

	UPROPERTY(ReplicatedUsing=OnRep_ChangeBuffState)
	EBuffState BuffState;

	UFUNCTION()
	void OnRep_ChangeBuffState();

	UFUNCTION(BlueprintImplementableEvent,Category="Buff")
	void OnBuffStateChanged(EBuffState NewBuffState);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//计数函数
	void TickCounter();
protected:
	
	//Buff的每次实际效果
	UFUNCTION(BlueprintImplementableEvent, Category="Buff")
	void TickBuff();

	//Buff总生效次数
	UPROPERTY(EditAnywhere, Category="Buff")
	int32 TotalTicks;

	//Tick间隔
	UPROPERTY(EditAnywhere, Category="Buff")
	float TickInterval;

	//计数值
	int32 TickProcessed;
	
	FTimerHandle TimerHandle;
};
