// Fill out your copyright notice in the Description page of Project Settings.


#include "Challenges/SExplosiveBarrel.h"

#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/RadialForceComponent.h"

ASExplosiveBarrel::ASExplosiveBarrel()
{
 	StaticMeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent=StaticMeshComp;

	HealthComp=CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RadialForceComp=CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->bImpulseVelChange=true;
	RadialForceComp->bIgnoreOwningActor=true;
	RadialForceComp->bAutoActivate=true;
	RadialForceComp->SetupAttachment(RootComponent);

	ExplosionImpulse=400.f;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	//只有服务端运行本函数
	if (bExploded)
		return;
	if (Health<=0)
	{
		bExploded=true;
		StaticMeshComp->AddImpulse(ExplosionImpulse*FVector::UpVector,NAME_None,true);
		RadialForceComp->FireImpulse();
		OnRep_Exploded();
	}
}

void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this,&ASExplosiveBarrel::OnHealthChanged);
	}
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplosionEffect,GetActorLocation());
	StaticMeshComp->SetMaterial(0,ExplodedMat);
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel,bExploded);
}



