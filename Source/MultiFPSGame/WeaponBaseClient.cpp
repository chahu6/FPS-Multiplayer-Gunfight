// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBaseClient.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AWeaponBaseClient::AWeaponBaseClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetOnlyOwnerSee(true);

	ShootSound = 0.1f;
}

// Called when the game starts or when spawned
void AWeaponBaseClient::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeaponBaseClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBaseClient::DisplayWeaponEffect()
{
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("Fire_FX_Slot"), FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::KeepRelativeOffset, true, EPSCPoolMethod::None, true);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound, ShootSound);
	}
}