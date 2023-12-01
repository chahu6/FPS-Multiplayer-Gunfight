// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseClient.h"
#include "WeaponBaseServer.generated.h"


UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	AK47 UMETA(DisplayName = "AK47"),
	M4A1 UMETA(DisplayName = "M4A1"),
	MP7  UMETA(DisplayName = "MP7"),
	SVD  UMETA(DisplayName = "SVD"),
	DesertEagle UMETA(DisplayName = "DesertEagle"),
};

UCLASS()
class MULTIFPSGAME_API AWeaponBaseServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBaseServer();

	UPROPERTY(EditAnywhere)
	EWeaponType KindOfWeapon;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereCollision;

	UFUNCTION()
	void OnOtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EquipWeapon();

	UPROPERTY(EditAnywhere)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly)
	int32 GunCurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Replicated)
	int32 ClipCurrentAmmo;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxClipAmmo;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* ServerTPBodysShootAnimMotage;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* ServerTPBodysReloadAnimMotage;

	UPROPERTY(EditAnywhere)
	float BullentDistance;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BulletDecal;

	UPROPERTY(EditAnywhere)
	float BaseDamage;

	UPROPERTY(EditAnywhere)
	bool IsAutomatic;

	UPROPERTY(EditAnywhere)
	float AutomaticFireRate;

	UPROPERTY(EditAnywhere)
	UCurveFloat* VerticalRecoilCurve;

	UPROPERTY(EditAnywhere)
	UCurveFloat* HorizontalRecoilCurve;

	UPROPERTY(EditAnywhere)
	float MovingFireRandomRange;

	UPROPERTY(EditAnywhere, Category="Pistol")
	float SpreadWeaponCallBackRate;

	UPROPERTY(EditAnywhere, Category = "Pistol")
	float SpreadWeaponMinIndex;

	UPROPERTY(EditAnywhere, Category = "Pistol")
	float SpreadWeaponMaxIndex;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWeaponBaseClient> ClientWeaponBaseBPClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiShooting();
	void MultiShooting_Implementation();
	bool MultiShooting_Validate();


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
