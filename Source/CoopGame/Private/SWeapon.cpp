// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

static int32 DebugWeaponDrawing=0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapon"),DebugWeaponDrawing,
	TEXT("WeaponTraceControl"),ECVF_Cheat);

ASWeapon::ASWeapon()
{
	SkeletalMeshComp=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Components"));
	RootComponent=SkeletalMeshComp;

	MuzzleSocketName="MuzzleSocket";
	TracerTargetName="Target";

	BaseDamage=20.f;
	RateOfFire=600.f;

	bReplicates=true;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	ShootsInterval=60/RateOfFire;
}

void ASWeapon::Fire()
{
	if (GetLocalRole()<ROLE_Authority)
	{
		ServerFire();
		return;
	}
	AActor*MyOwner=GetOwner();
	if(MyOwner)
	{
		FHitResult Hit;
		
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);
		FVector EndLocation=EyeLocation+(EyeRotation.Vector()*10000.f);
		FVector TraceEnd=EndLocation;
		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(MyOwner);
		Params.AddIgnoredActor(this);
		Params.bTraceComplex=true;
		Params.bReturnPhysicalMaterial=true;
		
		EPhysicalSurface SurfaceType=EPhysicalSurface::SurfaceType_Default;
		//如果命中,更新终点、打击点材质、实际伤害,应用点伤害
		if(GetWorld()->LineTraceSingleByChannel(Hit,EyeLocation,EndLocation,COLLISION_WEAPON,Params))
		{
			TraceEnd=Hit.ImpactPoint;
			SurfaceType=UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			float ActualDamage=BaseDamage;
			if(SurfaceType==SURFACE_FLESHVULNERABLE)ActualDamage*=4;
			
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(),ActualDamage,EyeLocation,Hit,MyOwner->GetInstigatorController(),this,DamageType);
		}
		//Debug控制
		if(DebugWeaponDrawing>0)
			DrawDebugLine(GetWorld(),EyeLocation,EndLocation,FColor::Red,false,1,0,1);
		//生成特效
		PlayFireEffects(TraceEnd,SurfaceType);
		//服务器更新结构体
		if (GetLocalRole()==ROLE_Authority)
		{
			HitScanTrace.BurstCounter++;
			HitScanTrace.TraceTo=TraceEnd;
			HitScanTrace.SurfaceType=SurfaceType;
		}
		//开枪时间留档
		LastShootTime=GetWorld()->TimeSeconds;
	}
}
void ASWeapon::PlayFireEffects(FVector TraceEnd,EPhysicalSurface SurfaceType)
{
	//生成开火特效
	if(MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect,SkeletalMeshComp,MuzzleSocketName);
	//生成弹道特效
	if(TracerEffect)
	{
		FVector MuzzleLocation=SkeletalMeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp=UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),TracerEffect,MuzzleLocation);
		if (TracerComp)
			TracerComp->SetVectorParameter(TracerTargetName,TraceEnd);
	}
	//打击点特效
	UParticleSystem*SelectEffect=nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectEffect=FleshImpactEffect;
		break;
	case SURFACE_FLESHVULNERABLE:
		SelectEffect=FleshImpactEffect;
		break;
	default:
		SelectEffect=DefaultImpactEffect;
		break;
	}
	if(SelectEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),SelectEffect,TraceEnd,FRotator::ZeroRotator);
			
	//镜头抖动特效 
	APawn*MyOwner=Cast<APawn>(GetOwner());
	if(MyOwner)
	{
		APlayerController*PC=Cast<APlayerController>(MyOwner->GetController());
		if (PC)
			PC->ClientStartCameraShake(FireCameraShake);
	}
}

void ASWeapon::StartFire()
{
	float FirstDelay=FMath::Max(0.f,ShootsInterval-GetWorld()->TimeSeconds+LastShootTime);
	GetWorldTimerManager().SetTimer(FireTimerHandle,this,&ASWeapon::Fire,ShootsInterval,true,FirstDelay);
}
void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ASWeapon::Rep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo,HitScanTrace.SurfaceType);
}
void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon,HitScanTrace,COND_SkipOwner);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}
bool ASWeapon::ServerFire_Validate()
{
	return true;
}


