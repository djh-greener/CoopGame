// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

USTRUCT()
struct FHitScanTrace//用于服务器向客户端同步开枪特效
{
	GENERATED_BODY()
public:
	UPROPERTY()//打击点位置
	FVector_NetQuantize TraceTo;

	UPROPERTY()//打击点物理材质
	TEnumAsByte<EPhysicalSurface>SurfaceType;
	
	UPROPERTY()//开枪数，使每次开枪此结构体都变动
	uint8 BurstCounter;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire();

	void PlayFireEffects(FVector TraceEnd,EPhysicalSurface SurfaceType);
public:
	UFUNCTION(Server,Reliable,WithValidation)
	void ServerFire();
	
	void StartFire();

	void StopFire();
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USkeletalMeshComponent*SkeletalMeshComp;
	//开火特效相关
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	FName MuzzleSocketName;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UParticleSystem*MuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UParticleSystem*DefaultImpactEffect;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UParticleSystem*FleshImpactEffect;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UParticleSystem*TracerEffect;
	
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	FName TracerTargetName;
	
	UPROPERTY(EditDefaultsOnly,Category="Player")
	TSubclassOf<UMatineeCameraShake>FireCameraShake;
	
	//伤害相关
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	TSubclassOf<UDamageType>DamageType;
	
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	float BaseDamage;

	//连续开火
	FTimerHandle FireTimerHandle;
	
	float LastShootTime;
	
	UPROPERTY(EditDefaultsOnly,Category="Weapon")
	float RateOfFire;
	
	float ShootsInterval;

	//网络属性回调
	UPROPERTY(ReplicatedUsing=Rep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()//此函数在接收更新的属性对应端执行
	void Rep_HitScanTrace();

};
