// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIFPSGAME_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void PlayCameraShake(TSubclassOf<class UCameraShakeBase> CameraShakeBase);

	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
	void CreatePlayerUI();

	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
	void DoCrosshairRecoil();

	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
	void UpdateAmmoUI(int32 ClipCurrentAmmo, int32 GunCurrentAmmo);

	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
	void UpdateHealthUI(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void DeathMatch(AActor* DamageActor);

	//∑œ¡À
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void UpdateCrosshair();
};
