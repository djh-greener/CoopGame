// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASWeapon;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 按键输入
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	void MoveForward(float value);
	void MoveRight(float value);

	void BeginCrouch();
	void EndCrouch();

protected://摄像机、视角与瞄准
	UFUNCTION(BlueprintImplementableEvent,Category="Weapon")
	void BeginZoom();

	UFUNCTION(BlueprintImplementableEvent,Category="Weapon")
	void EndZoom();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class UCameraComponent *CameraComp;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USpringArmComponent* SpringArmComp;

public://生命相关
	UFUNCTION()
	void OnHealthChanged(class USHealthComponent* HealthComp,float Health,float HealthDelta,
		const class UDamageType*  DamageType, class AController*  InstigatedBy, AActor*  DamageCauser);
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USHealthComponent* HealthComponent;
	
	UPROPERTY(Replicated,EditDefaultsOnly,BlueprintReadOnly,Category="Player")
	bool bDied;
	
protected://武器，开火相关
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StartFire();
	void StopFire();

	UPROPERTY(Replicated)
	ASWeapon*CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	TSubclassOf<ASWeapon>StartWeaponClass;
	
	UPROPERTY(VisibleDefaultsOnly,Category="Weapon")
	FName WeaponAttachSocketName;
	
public://Buff相关
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Buff")
	class UBuffManager*BuffManager;
	
public://其他
	virtual FVector GetPawnViewLocation() const override;
};
