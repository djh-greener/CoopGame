// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class ASPowerUpActor;

UCLASS()
class COOPGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();
protected:
	UPROPERTY(VisibleDefaultsOnly,Category="Components")
	class USphereComponent*SphereComp;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="PickupActor")
	UStaticMeshComponent*StaticMeshComp;

	UPROPERTY(EditInstanceOnly,Category="PickupActor")
	class UPointLightComponent*PointLightComp;
	
	UPROPERTY(VisibleDefaultsOnly,Category="Components")
	UDecalComponent*DecalComp;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly,Category="PickupActor")
	TSubclassOf<class ABuff>BuffClass;

	UPROPERTY(Replicated)
	ABuff*BuffInst;
	
	UPROPERTY(EditInstanceOnly,Category="PickupActor")
	float CoolDownDuration;
	
	FTimerHandle TimerHandle_ReSpawnTimer;

	void ReSpawn();
public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
