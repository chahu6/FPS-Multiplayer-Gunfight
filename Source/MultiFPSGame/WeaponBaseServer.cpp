// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBaseServer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBaseServer::AWeaponBaseServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	WeaponMesh->SetOwnerNoSee(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetSimulatePhysics(true);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBaseServer::OnOtherBeginOverlap);

	bReplicates = true;
	//SetReplicates(true);//Ì«Âý

	BaseDamage = 25.0f;

	IsAutomatic = true;

	AutomaticFireRate = 0.12f;

	MovingFireRandomRange = 1500.0f;
}

void AWeaponBaseServer::OnOtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(OtherActor);
	if (FPSCharacter)
	{
		EquipWeapon();
		if (KindOfWeapon == EWeaponType::DesertEagle)
		{
			FPSCharacter->EquipSecondary(this);
		}
		else
		{
			FPSCharacter->EquipPrimary(this);
		}
	}
}

void AWeaponBaseServer::EquipWeapon()
{
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeaponBaseServer::BeginPlay()
{
	Super::BeginPlay();

	//SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBaseServer::OnOtherBeginOverlap);
}

// Called every frame
void AWeaponBaseServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBaseServer::MultiShooting_Implementation()
{
	if (GetOwner() != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		FName MuzzleFlashSocketName = TEXT("Fire_FX_Slot");
		if (MuzzleFlash)
		{
			if (KindOfWeapon == EWeaponType::M4A1)
			{
				MuzzleFlashSocketName = TEXT("MuzzleSocket");
			}
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, MuzzleFlashSocketName, FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::KeepRelativeOffset, true, EPSCPoolMethod::None, true);
		}

		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
		}
	}
}

bool AWeaponBaseServer::MultiShooting_Validate()
{
	return true;
}

void AWeaponBaseServer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBaseServer, ClipCurrentAmmo);
}

