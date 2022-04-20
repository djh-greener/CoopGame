// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"

void ASProjectileWeapon::Fire()
{
	AActor*MyOwner=GetOwner();
	if(MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);
		FVector MuzzleLocation=SkeletalMeshComp->GetSocketLocation(MuzzleSocketName);
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (ProjectileClass)
			GetWorld()->SpawnActor<AActor>(ProjectileClass,MuzzleLocation,EyeRotation,SpawnParameters);
	}
}
