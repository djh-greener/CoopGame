// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/STracerBor.h"

#include "DrawDebugHelpers.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASTracerBor::ASTracerBor()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	SetRootComponent(MeshComp);

	HealthComp=CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	
	SphereComp=CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200.f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);
	
	MovementForce=1000.f;
	bUseVelocityChange=false;
	RequiredDistanceToTarget=100.f;

	ExplosionRadius=200.f;
	ExplosionDamage=100.f;

	bStartSelfDestruction=false;
}

// Called when the game starts or when spawned
void ASTracerBor::BeginPlay()
{
	Super::BeginPlay();
	if(GetLocalRole()==ROLE_Authority)
	{
		NextPathPoint=GetNextPathPoint();
		FTimerHandle myTimerHandle;
		GetWorldTimerManager().SetTimer(myTimerHandle,this,&ASTracerBor::OnCheckNearbyBots,1,true);
	}
		
	HealthComp->OnHealthChanged.AddDynamic(this,&ASTracerBor::HandleTakeDamage);
}

// Called every frame
void ASTracerBor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugSphere(GetWorld(),NextPathPoint,20,12,FColor::Yellow,false,0,0,1);
	if(GetLocalRole()<ROLE_Authority||bExploded)return;
	
	FVector ForceDirection=NextPathPoint-GetActorLocation();
	float DistanceToTarget=ForceDirection.Size();
	if(DistanceToTarget>RequiredDistanceToTarget)
	{
		ForceDirection.Normalize();
		ForceDirection*=MovementForce;
		MeshComp->AddImpulse(ForceDirection,NAME_None,bUseVelocityChange);
		DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(),GetActorLocation()+ForceDirection,
			32,FColor::Red,false,0,0,1);
	}
	else
	{
		NextPathPoint=GetNextPathPoint();
	}
}

void ASTracerBor::HandleTakeDamage(USHealthComponent* OwnerHealthComp, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst==nullptr)
		MatInst=MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0,MeshComp->GetMaterial(0));
	
	if (MatInst)
		MatInst->SetScalarParameterValue("LastTimeDamageTaken",GetWorld()->TimeSeconds);
	
	if (Health<=0)
		SelfDestruct();
}

void ASTracerBor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartSelfDestruction)
	{
		APawn*PlayerPawn=Cast<APawn>(OtherActor);
		if (PlayerPawn)
		{
			if(GetLocalRole()==ROLE_Authority)
				GetWorldTimerManager().SetTimer(TimerHandle,this,&ASTracerBor::DamageSelf,0.5f,true,0);
			bStartSelfDestruction=true;
			if (SelfDestructSound)
				UGameplayStatics::SpawnSoundAttached(SelfDestructSound,RootComponent);
		}
	}
}

void ASTracerBor::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this,20.f,GetInstigatorController(),this,nullptr);
}

void ASTracerBor::SelfDestruct()
{
	if(bExploded)return;
	
	bExploded=true;
	
	if(ExplosionEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplosionEffect,GetActorLocation());

	if (ExplosionSound)
		UGameplayStatics::SpawnSoundAttached(ExplosionSound,RootComponent);

	if(GetLocalRole()==ROLE_Authority)
	{
		TArray<AActor*>IgnoredActors;
		IgnoredActors.Add(this);
		float RealDamage=ExplosionDamage+ExplosionDamage*PowerLevel;
		UGameplayStatics::ApplyRadialDamage(GetWorld(),RealDamage,GetActorLocation(),ExplosionRadius,
			nullptr,IgnoredActors,this,GetInstigatorController(),true);
		SetLifeSpan(2.0f);
	}
	DrawDebugSphere(GetWorld(),GetActorLocation(),ExplosionRadius,12,FColor::Green,false,2,0,1);

	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetVisibility(false,true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FVector ASTracerBor::GetNextPathPoint()
{
	APawn*PlayerPawn=UGameplayStatics::GetPlayerPawn(this,0);
	if (PlayerPawn)
	{
		UNavigationPath*NavigationPath=UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),GetActorLocation(),PlayerPawn);
		if (NavigationPath&&NavigationPath->PathPoints.Num()>1)
		{
			return NavigationPath->PathPoints[1];
		}
	}
	
	return GetActorLocation();
}

void ASTracerBor::OnCheckNearbyBots()
{
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(600.f);

	TArray<FOverlapResult>OverlapResults;
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	GetWorld()->OverlapMultiByObjectType(OverlapResults,GetActorLocation(),FQuat::Identity,QueryParams,CollisionShape);
	DrawDebugSphere(GetWorld(),GetActorLocation(),CollisionShape.GetSphereRadius(),12,FColor::Blue,false,1);

	int32 NumBots=0;
	for (auto it:OverlapResults)
	{
		ASTracerBor*Bot=Cast<ASTracerBor>(it.GetActor());
		if (Bot&&Bot!=this)
		{
			NumBots++;
		}
	}
	const int MaxPowerLevel=4;
	PowerLevel=FMath::Clamp(NumBots,0,MaxPowerLevel);

	if (MatInst==nullptr)
	{
		MatInst=MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0,MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		float Alpha=PowerLevel/(float)MaxPowerLevel;
		MatInst->SetScalarParameterValue("PowerLevelAlpha",Alpha);
	}
}
