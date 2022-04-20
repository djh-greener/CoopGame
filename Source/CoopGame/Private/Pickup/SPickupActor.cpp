// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/SPickupActor.h"

#include "SCharacter.h"
#include "Components/BuffManager.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "CoopGame/CoopGame.h"
#include "GameFramework/Character.h"
#include "Pickup/Buff.h"
#include "Components/PointLightComponent.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp=CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.f);
	SetRootComponent(SphereComp);
	
	DecalComp=CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90,0,0));
	DecalComp->DecalSize=FVector(64,75,75);
	DecalComp->SetupAttachment(RootComponent);

	StaticMeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetupAttachment(RootComponent);

	PointLightComp=CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComp"));
	PointLightComp->SetupAttachment(StaticMeshComp);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	ReSpawn();
}

void ASPickupActor::ReSpawn()
{
	if (!BuffClass)
	{
		DDH::Warning()<<"No BuffClass"<<DDH::Endl();
		return;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BuffInst=GetWorld()->SpawnActor<ABuff>(BuffClass,GetTransform(),SpawnParameters);
	StaticMeshComp->SetVisibility(true);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ASCharacter*PP=Cast<ASCharacter>(OtherActor);
	
	if (PP){
		if (BuffInst){
			PP->BuffManager->BuffTable["Active"].Add(BuffInst);
			StaticMeshComp->SetVisibility(false);
			BuffInst=nullptr;
			GetWorldTimerManager().SetTimer(TimerHandle_ReSpawnTimer,this,&ASPickupActor::ReSpawn,CoolDownDuration);
		}
	}
}


