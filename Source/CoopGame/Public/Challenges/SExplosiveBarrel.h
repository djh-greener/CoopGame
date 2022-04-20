// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SExplosiveBarrel.generated.h"

class USHealthComponent;
class URadialForceComponent;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent,float Health,float HealthDelta,
		const class UDamageType*  DamageType, class AController*  InstigatedBy, AActor*  DamageCauser);

protected:
	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent*StaticMeshComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USHealthComponent*HealthComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	URadialForceComponent*RadialForceComp;

	UPROPERTY(EditDefaultsOnly,Category="FX")
	float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly,Category="FX")
	UMaterialInterface*ExplodedMat;

	UPROPERTY(EditDefaultsOnly,Category="FX")
	UParticleSystem*ExplosionEffect;
public:
	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	UFUNCTION()
	void OnRep_Exploded();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
