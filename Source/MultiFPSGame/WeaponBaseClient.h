// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseClient.generated.h"

UCLASS()
class MULTIFPSGAME_API AWeaponBaseClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBaseClient();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* ClientArmsFireAnimMontage;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* ClientArmsReloadAnimMontage;

	UPROPERTY(EditAnywhere)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere)
	float ShootSound;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	UPROPERTY(EditAnywhere)
	int FPArmsBlendPose;

	UPROPERTY(EditAnywhere)
	float FieldOfAimingView;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintImplementableEvent, Category = "FPGunAnimation")
	void PlayShootAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "FPGunAnimation")
	void PlayReloadAnimation();

	void DisplayWeaponEffect();
};
