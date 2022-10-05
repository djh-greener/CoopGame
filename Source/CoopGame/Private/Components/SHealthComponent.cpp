// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"

#include "CoopGame/CoopGame.h"
#include "GamePlay/SGameMode.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth=100;
	bIsDead=false;
	SetIsReplicated(true);
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health=DefaultHealth;

	if (GetOwnerRole()==ROLE_Authority)//服务器处理伤害
	{
		AActor*MyOwner=GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this,&USHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent,Health);
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                            AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead)
		return;
	Health=FMath::Clamp(Health-Damage,0.f,DefaultHealth);
	bIsDead=Health<=0;
	DDH::Warning()<<"Health :"<<Health<<DDH::Endl();
	OnHealthChanged.Broadcast(this,Health,Damage,DamageType,InstigatedBy,DamageCauser);

	if (bIsDead)
	{
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
	}
}

void USHealthComponent::On_RepHealth(float OldHealth)
{
	OnHealthChanged.Broadcast(this,Health,OldHealth-Health,nullptr,nullptr,nullptr);
}

float USHealthComponent::GetHealth() const
{
	return Health;
}

void USHealthComponent::Heal(float Amount)
{
	if(Health<=0||Amount<=0)
		return;
	Health=FMath::Clamp(Health+Amount,0.f,DefaultHealth);

	OnHealthChanged.Broadcast(this,Health,-Amount,nullptr,nullptr,nullptr);
}

