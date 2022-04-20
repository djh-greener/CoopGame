// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "CoopGame/CoopGame.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth=100;

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
	if (Damage<=0)
		return;
	Health=FMath::Clamp(Health-Damage,0.f,DefaultHealth);
	DDH::Warning()<<"Health :"<<Health<<DDH::Endl();
	OnHealthChanged.Broadcast(this,Health,Damage,DamageType,InstigatedBy,DamageCauser);
}

void USHealthComponent::On_RepHealth(float OldHealth)
{
	OnHealthChanged.Broadcast(this,Health,OldHealth-Health,nullptr,nullptr,nullptr);
}

void USHealthComponent::Heal(float Amount)
{
	if(Health<=0||Amount<=0)
		return;
	Health=FMath::Clamp(Health+Amount,0.f,DefaultHealth);

	OnHealthChanged.Broadcast(this,Health,-Amount,nullptr,nullptr,nullptr);
}
