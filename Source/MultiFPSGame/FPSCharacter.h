// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBaseServer.h"
#include "WeaponBaseClient.h"
#include "FPSCharacter.generated.h"

UCLASS()
class MULTIFPSGAME_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	float Health;
	float DefaultHealth;

	float PistolSpreadMin = 0.0f;
	float PistolSpreadMax = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	bool Death;

	UFUNCTION()
	void DelayBeginPlayCallBack();

#pragma region Component
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class AFPSPlayerController* FPC;

	UPROPERTY(EditAnywhere)
	EWeaponType TestStartWeapon;
#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region InputEvent
	void MoveForward(float value);
	void MoveRight(float value);

	void StartJump();
	void StopJump();
	void LowSpeedWalk();
	void NormalSpeedWalk();

	void InputFirePressed();
	void InputFireReleased();

	//����
	void InputAimingPressed();
	void InputAimingReleased();

	void InputReload();
#pragma endregion


#pragma region Weapon
public:
	void EquipPrimary(AWeaponBaseServer* weaponBaseServer);

	void EquipSecondary(AWeaponBaseServer* weaponBaseServer);

	AWeaponBaseClient* GetCurrentClientFPArmsWeaponActor();
	AWeaponBaseServer* GetCurrentServerTPBodysWeaponActor();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateFPArmsBlendPose(int NewIndex);

	

private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), Replicated, BlueprintReadOnly)
	EWeaponType ActiveWeapon;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	AWeaponBaseServer* ServerPrimaryWeapon;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	AWeaponBaseServer* ServerSecondaryWeapon;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	AWeaponBaseClient* ClientPrimaryWeapon;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	AWeaponBaseClient* ClientSecondaryWeapon;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAnimInstance* ClientArmsAnimBP;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAnimInstance* ServerBodysAnimBP;

	void StartWithKindOfWeapon();

	void PurchaseWeapon(EWeaponType WeaponType);

#pragma endregion
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
#pragma region NetWorking
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLowSpeedWalk();
	void ServerLowSpeedWalk_Implementation();
	bool ServerLowSpeedWalk_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNormalSpeedWalk();
	void ServerNormalSpeedWalk_Implementation();
	bool ServerNormalSpeedWalk_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireRifleWeapon(FVector CameraVector, FRotator CameraRotation, bool IsMoving);
	void ServerFireRifleWeapon_Implementation(FVector CameraVector, FRotator CameraRotation, bool IsMoving);
	bool ServerFireRifleWeapon_Validate(FVector CameraVector, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireSniperWeapon(FVector CameraVector, FRotator CameraRotation, bool IsMoving);
	void ServerFireSniperWeapon_Implementation(FVector CameraVector, FRotator CameraRotation, bool IsMoving);
	bool ServerFireSniperWeapon_Validate(FVector CameraVector, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFirePistolWeapon(FVector CameraVector, FRotator CameraRotation, bool IsMoving);
	void ServerFirePistolWeapon_Implementation(FVector CameraVector, FRotator CameraRotation, bool IsMoving);
	bool ServerFirePistolWeapon_Validate(FVector CameraVector, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiShooting();
	void MultiShooting_Implementation();
	bool MultiShooting_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiReloadAnimation();
	void MultiReloadAnimation_Implementation();
	bool MultiReloadAnimation_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiSpawnBulletDecal(FVector Location, FRotator Rotation);
	void MultiSpawnBulletDecal_Implementation(FVector Location, FRotator Rotation);
	bool MultiSpawnBulletDecal_Validate(FVector Location, FRotator Rotation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiDeathMatch();
	void MultiDeathMatch_Implementation();
	bool MultiDeathMatch_Validate();
	
	//����
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadPrimary();
	void ServerReloadPrimary_Implementation();
	bool ServerReloadPrimary_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadSecondary();
	void ServerReloadSecondary_Implementation();
	bool ServerReloadSecondary_Validate();

	//ֹͣ���
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate();

	//����
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAiming(bool AimingState);
	void ServerSetAiming_Implementation(bool AimingState);
	bool ServerSetAiming_Validate(bool AimingState);

	UFUNCTION(Client, Reliable)
	void ClientEquipFPArmsPrimary();

	UFUNCTION(Client, Reliable)
	void ClientEquipFPArmsSecondary();

	UFUNCTION(Client, Reliable)
	void ClientFire();

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmoUI(int32 ClipCurrentAmmo, int32 GunCurrentAmmo);

	UFUNCTION(Client, Reliable)
	void ClientUpdateHealthUI(float NewHeallth);

	UFUNCTION(Client, Reliable)
	void ClientRecoil();

	UFUNCTION(Client, Reliable)
	void ClientReload();

	UFUNCTION(Client, Reliable)
	void ClientAiming();

	UFUNCTION(Client, Reliable)
	void ClientEndAiming();

	//����
	UFUNCTION(Client, Reliable)
	void ClientUpdateCrosshair();
#pragma endregion
#pragma region Fire
public:
	//��ʱ��
	FTimerHandle AutomaticFireTimerHandle;
	void AutomaticFire();

	//������
	float NewVerticalRecoilAmount;
	float OldVerticalRecoilAmount;
	float VerticalRecoilAmount;
	float RecoilXCoordPerShoot;
	void ResetRecoil();
	float NewHorizontalRecoilAmount;
	float OldHorizontalRecoilAmount;
	float HorizontalRecoilAmount;

	//����Reload
	UPROPERTY(Replicated)
	bool IsFiring;
	UPROPERTY(Replicated)
	bool IsReloading;

	UFUNCTION()
	void DelayPlayArmReloadCallBack();

	//��ǹ������
	void FireWeaponPrimary();
	void StopFirePrimary();
	void RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	//�ѻ�ǹ������
	void FireWeaponSniper();
	void StopFireSniper();
	void SniperLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	UPROPERTY(Replicated)
	bool IsAiming;

	UFUNCTION()
	void DelaySniperShootCallBack();

	UPROPERTY(VisibleAnywhere, Category = "SniperUI")
	class UUserWidget* WidgetScope;

	UPROPERTY(EditAnywhere, Category = "SniperUI")
	TSubclassOf<UUserWidget> SniperScopeBPClass;
	
	//��ǹ������
	void FireWeaponSecondary();
	void StopFireSecondary();
	void PistolLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	UFUNCTION()
	void DelaySpreadWeaponShootCallBack();

	void DamagePlayer(UPhysicalMaterial* PhysicalMaterial, AActor* DamagedActor, const FVector& HitFromDirection, const FHitResult& HitInfo);

	UFUNCTION()
	void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void DeathMatch(AActor* DamageActor);
#pragma endregion

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
