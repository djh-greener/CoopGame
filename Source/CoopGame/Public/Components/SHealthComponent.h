// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature,
	class USHealthComponent*,HealthComp,
	float,Health,
	float,HealthDelta,
	const class UDamageType*, DamageType,
	class AController*, InstigatedBy,
	AActor*, DamageCauser);

UCLASS( ClassGroup=(Coop), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="HealthComponent")
	float DefaultHealth;
	
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

public:
	UPROPERTY(ReplicatedUsing=On_RepHealth,VisibleDefaultsOnly,BlueprintReadOnly,Category="HealthComponent")
	float Health;
	
	UFUNCTION()
	void On_RepHealth(float OldHealth);

	UFUNCTION(BlueprintCallable,Category="HealthComponent")
	void Heal(float Amount);
	
	UPROPERTY(BlueprintAssignable,Category="Events")
	FOnHealthChangedSignature OnHealthChanged;
};
