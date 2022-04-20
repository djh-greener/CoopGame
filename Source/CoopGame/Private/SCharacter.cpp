// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SWeapon.h"
#include "CoopGame/CoopGame.h"
#include "Net/UnrealNetwork.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BuffManager.h"

// Sets default values
ASCharacter::ASCharacter()
{
	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation=true;
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp,USpringArmComponent::SocketName);

	HealthComponent=CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	
	BuffManager=CreateDefaultSubobject<UBuffManager>(TEXT("BuffManager"));
	
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch=true;
	
	WeaponAttachSocketName="WeaponSocket";
	CurrentWeapon=nullptr;
	bDied=false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->OnHealthChanged.AddDynamic(this,&ASCharacter::OnHealthChanged);
	if(GetLocalRole()==ROLE_Authority)//武器在服务器生成
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon=GetWorld()->SpawnActor<ASWeapon>(StartWeaponClass,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
		if(CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,WeaponAttachSocketName);
		}
	}
}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector(),value);
}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector(),value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::StartFire()
{
	if(CurrentWeapon)
		CurrentWeapon->StartFire();
	else
		DDH::Debug()<<"ASCharacter::StartFire"<<DDH::Endl();
}

void ASCharacter::StopFire()
{
	if(CurrentWeapon)
		CurrentWeapon->StopFire();
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp",this,&ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn",this,&ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch",EInputEvent::IE_Pressed,this,&ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch",EInputEvent::IE_Released,this,&ASCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump",EInputEvent::IE_Released,this,&ASCharacter::Jump);

	PlayerInputComponent->BindAction("Zoom",EInputEvent::IE_Pressed,this,&ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom",EInputEvent::IE_Released,this,&ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire",EInputEvent::IE_Pressed,this,&ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire",EInputEvent::IE_Released,this,&ASCharacter::StopFire);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if(CameraComp)
		return CameraComp->GetComponentLocation();
	return Super::GetPawnViewLocation();
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health<=0&&!bDied)//将死之人
	{
		bDied=true;
		GetCharacterMovement()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter,CurrentWeapon);
	DOREPLIFETIME(ASCharacter,bDied);
}



