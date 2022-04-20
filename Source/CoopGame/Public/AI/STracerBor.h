// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "STracerBor.generated.h"

class USoundCue;

UCLASS()
class COOPGAME_API ASTracerBor : public APawn
{
	GENERATED_BODY()

public:
	ASTracerBor();
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent*MeshComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USHealthComponent*HealthComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USphereComponent*SphereComp;
	
protected:
	virtual void BeginPlay() override;
	
protected://受伤反应
	UFUNCTION()
	void HandleTakeDamage(class USHealthComponent* OwnerHealthComp,float Health,float HealthDelta,
		const class UDamageType*  DamageType, class AController*  InstigatedBy, AActor*  DamageCauser);
	
	class UMaterialInstanceDynamic*MatInst;

protected://靠近自残
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	void DamageSelf();

	FTimerHandle TimerHandle;

	bool bStartSelfDestruction;

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	USoundCue* SelfDestructSound;
	
	
protected://自爆
	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	class UParticleSystem*ExplosionEffect;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	USoundCue* ExplosionSound;
	
public:	//自动追踪
	virtual void Tick(float DeltaTime) override;
protected:
	FVector GetNextPathPoint();

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	float MovementForce;
	
	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly,Category="TracerBot")
	float RequiredDistanceToTarget;
	
	FVector NextPathPoint;

protected:
	UFUNCTION()
	void OnCheckNearbyBots();
	
	int32 PowerLevel;
};
