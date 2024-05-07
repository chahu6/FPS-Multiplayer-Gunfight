// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponBaseServer.h"
#include "WeaponBaseClient.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Component
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	if (PlayerCamera != nullptr)
	{
		PlayerCamera->SetupAttachment(RootComponent);
		PlayerCamera->bUsePawnControlRotation = true;
	}
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	if (ArmsMesh != nullptr)
	{
		ArmsMesh->SetupAttachment(PlayerCamera);
		ArmsMesh->SetOnlyOwnerSee(true);
		ArmsMesh->bCastDynamicShadow = false;
		ArmsMesh->CastShadow = false;
	}
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
#pragma endregion
}

void AFPSCharacter::DelayBeginPlayCallBack()
{
	FPC = Cast<AFPSPlayerController>(GetController());
	if (FPC)
	{
		FPC->CreatePlayerUI();
	}
	else
	{
		FLatentActionInfo ActionInfo;
		ActionInfo.CallbackTarget = this;
		ActionInfo.ExecutionFunction = TEXT("DelayBeginPlayCallBack");
		ActionInfo.Linkage = 0;
		ActionInfo.UUID = FMath::Rand();
		UKismetSystemLibrary::Delay(this, 0.5f, ActionInfo);
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultHealth = Health = 100.0f;

	IsFiring = false;
	IsReloading = false;
	IsAiming = false;

	Death = false;

	OnTakePointDamage.AddDynamic(this, &AFPSCharacter::OnHit);

	FPC = Cast<AFPSPlayerController>(GetController());
	if (FPC)
	{
		FPC->CreatePlayerUI();
	}
	else
	{
		FLatentActionInfo ActionInfo;
		ActionInfo.CallbackTarget = this;
		ActionInfo.ExecutionFunction = TEXT("DelayBeginPlayCallBack");
		ActionInfo.Linkage = 0;
		ActionInfo.UUID = FMath::Rand();
		UKismetSystemLibrary::Delay(this, 0.5f, ActionInfo);
	}

	ClientArmsAnimBP = ArmsMesh->GetAnimInstance();

	ServerBodysAnimBP = GetMesh()->GetAnimInstance();

	StartWithKindOfWeapon();
}

#pragma region InputEvent
void AFPSCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector(), value);
}

void AFPSCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector(), value);
}
void AFPSCharacter::StartJump()
{
	Jump();
}
void AFPSCharacter::StopJump()
{
	StopJumping();
}
void AFPSCharacter::LowSpeedWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	ServerLowSpeedWalk();
}
void AFPSCharacter::NormalSpeedWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	ServerNormalSpeedWalk();
}
void AFPSCharacter::InputFirePressed()
{
	switch (ActiveWeapon)
	{
		case EWeaponType::AK47:
		{
			FireWeaponPrimary();
		}
		break;
		case EWeaponType::M4A1:
		{
			FireWeaponPrimary();
		}
		break;
		case EWeaponType::MP7:
		{
			FireWeaponPrimary();
		}
		break;
		case EWeaponType::DesertEagle:
		{
			FireWeaponSecondary();
		}
		break;
		case EWeaponType::SVD:
		{
			FireWeaponSniper();
		}
		break;
	}
}
void AFPSCharacter::InputFireReleased()
{
	switch (ActiveWeapon)
	{
		case EWeaponType::AK47:
		{
			StopFirePrimary();
		}
		break;
		case EWeaponType::M4A1:
		{
			StopFirePrimary();
		}
		break;
		case EWeaponType::MP7:
		{
			StopFirePrimary();
		}
		break;
		case EWeaponType::DesertEagle:
		{
			StopFireSecondary();
		}
		break;
		case EWeaponType::SVD:
		{
			StopFireSniper();
		}
		break;
	default:
		break;
	}
}

void AFPSCharacter::InputAimingPressed()
{
	//����׼����ǹ�����ò��ɼ�������ͷ�ɼ�������Զ�����ͻ���RPC
	//����IsAiming���ԡ���������RPC
	if (ActiveWeapon == EWeaponType::SVD)
	{
		ServerSetAiming(true);
		ClientAiming();
	}
}
void AFPSCharacter::InputAimingReleased()
{
	//ɾ����׼����ǹ����Ϊ�ɼ�������ͷ�ɼ���������
	if (ActiveWeapon == EWeaponType::SVD)
	{
		ServerSetAiming(false);
		ClientEndAiming();
	}
}

void AFPSCharacter::InputReload()
{
	if (!IsReloading)
	{
		if (!IsFiring)
		{
			switch (ActiveWeapon)
			{
				case EWeaponType::AK47:
				{
					ServerReloadPrimary();
				}
				break;
				case EWeaponType::M4A1:
				{
					ServerReloadPrimary();
				}
				break;
				case EWeaponType::MP7:
				{
					ServerReloadPrimary();
				}
				break;
				case EWeaponType::DesertEagle:
				{
					ServerReloadSecondary();
				}
				break;
				case EWeaponType::SVD:
				{
					ServerReloadPrimary();
				}
				break;
			}
		}
	}
}
#pragma endregion

#pragma region Weapon

void AFPSCharacter::EquipPrimary(AWeaponBaseServer* weaponBaseServer)
{
	if (ServerPrimaryWeapon)
	{

	}
	else
	{
		ServerPrimaryWeapon = weaponBaseServer;
		ServerPrimaryWeapon->SetOwner(this);
		ServerPrimaryWeapon->K2_AttachToComponent(GetMesh(), TEXT("Weapon_Rifle"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		ActiveWeapon = ServerPrimaryWeapon->KindOfWeapon;
		//if (IsLocallyControlled())
		//{
		ClientEquipFPArmsPrimary();
		//}
	}
}

void AFPSCharacter::EquipSecondary(AWeaponBaseServer* weaponBaseServer)
{
	if (ServerSecondaryWeapon)
	{

	}
	else
	{
		ServerSecondaryWeapon = weaponBaseServer;
		ServerSecondaryWeapon->SetOwner(this);
		ServerSecondaryWeapon->K2_AttachToComponent(GetMesh(), TEXT("Weapon_Rifle"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		ActiveWeapon = ServerSecondaryWeapon->KindOfWeapon;
		//if (IsLocallyControlled())
		//{
		ClientEquipFPArmsSecondary();
		//}
	}
}

void AFPSCharacter::StartWithKindOfWeapon()
{
	if (HasAuthority())
	{
		PurchaseWeapon(static_cast<EWeaponType>(UKismetMathLibrary::RandomIntegerInRange(0, 4)));
	}
}

void AFPSCharacter::PurchaseWeapon(EWeaponType WeaponType)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	switch (WeaponType)
	{
		case EWeaponType::AK47:
			{
			//��̬�õ�AK47 Server��
			UClass* BlueprintVar = StaticLoadClass(AWeaponBaseServer::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/AK_47/ServerBP_AK47.ServerBP_AK47_C'"));
			AWeaponBaseServer* ServerWeapon = GetWorld()->SpawnActor<AWeaponBaseServer>(BlueprintVar, GetActorTransform(), SpawnInfo);
			ServerWeapon->EquipWeapon();
			EquipPrimary(ServerWeapon);
			}
			break;
		case EWeaponType::M4A1:
			{
				//��̬�õ�AK47 Server��
				UClass* BlueprintVar = StaticLoadClass(AWeaponBaseServer::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/M4A1/ServerBP_M4A1.ServerBP_M4A1_C'"));
				AWeaponBaseServer* ServerWeapon = GetWorld()->SpawnActor<AWeaponBaseServer>(BlueprintVar, GetActorTransform(), SpawnInfo);
				ServerWeapon->EquipWeapon();
				EquipPrimary(ServerWeapon);
			}
			break;
		case EWeaponType::MP7:
			{
				//��̬�õ�AK47 Server��
				UClass* BlueprintVar = StaticLoadClass(AWeaponBaseServer::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/MP7/ServerBP_MP7.ServerBP_MP7_C'"));
				AWeaponBaseServer* ServerWeapon = GetWorld()->SpawnActor<AWeaponBaseServer>(BlueprintVar, GetActorTransform(), SpawnInfo);
				ServerWeapon->EquipWeapon();
				EquipPrimary(ServerWeapon);
			}
			break;
		case EWeaponType::DesertEagle:
			{
				//��̬�õ�AK47 Server��
				UClass* BlueprintVar = StaticLoadClass(AWeaponBaseServer::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/DesertEagle/ServerBP_DesertEagle.ServerBP_DesertEagle_C'"));
				AWeaponBaseServer* ServerWeapon = GetWorld()->SpawnActor<AWeaponBaseServer>(BlueprintVar, GetActorTransform(), SpawnInfo);
				ServerWeapon->EquipWeapon();
				EquipSecondary(ServerWeapon);
			}
			break;
		case EWeaponType::SVD:
		{
			//��̬�õ�AK47 Server��
			UClass* BlueprintVar = StaticLoadClass(AWeaponBaseServer::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/SVD/ServerBP_SVD.ServerBP_SVD_C'"));
			AWeaponBaseServer* ServerWeapon = GetWorld()->SpawnActor<AWeaponBaseServer>(BlueprintVar, GetActorTransform(), SpawnInfo);
			ServerWeapon->EquipWeapon();
			EquipPrimary(ServerWeapon);
		}
		break;
		default:
			break;
	}
}

AWeaponBaseClient* AFPSCharacter::GetCurrentClientFPArmsWeaponActor()
{
	switch (ActiveWeapon)
	{
		case EWeaponType::AK47:
		{
			return ClientPrimaryWeapon;
		}
		case EWeaponType::M4A1:
		{
			return ClientPrimaryWeapon;
		}
		case EWeaponType::MP7:
		{
			return ClientPrimaryWeapon;
		}
		case EWeaponType::DesertEagle:
		{
			return ClientSecondaryWeapon;
		}
		case EWeaponType::SVD:
		{
			return ClientPrimaryWeapon;
		}
	}
	return nullptr;
}

AWeaponBaseServer* AFPSCharacter::GetCurrentServerTPBodysWeaponActor()
{
	switch (ActiveWeapon)
	{
		case EWeaponType::AK47:
		{
			return ServerPrimaryWeapon;
		}
		case EWeaponType::M4A1:
		{
			return ServerPrimaryWeapon;
		}
		case EWeaponType::MP7:
		{
			return ServerPrimaryWeapon;
		}
		case EWeaponType::DesertEagle:
		{
			return ServerSecondaryWeapon;
		}
		case EWeaponType::SVD:
		{
			return ServerPrimaryWeapon;
		}
	}
	return nullptr;
}

#pragma endregion

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("LowSpeedWalk"), IE_Pressed, this, &AFPSCharacter::LowSpeedWalk);
	PlayerInputComponent->BindAction(TEXT("LowSpeedWalk"), IE_Released, this, &AFPSCharacter::NormalSpeedWalk);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFPSCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AFPSCharacter::StopJumping);
	
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AFPSCharacter::InputFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AFPSCharacter::InputFireReleased);

	PlayerInputComponent->BindAction(TEXT("Aiming"), IE_Pressed, this, &AFPSCharacter::InputAimingPressed);
	PlayerInputComponent->BindAction(TEXT("Aiming"), IE_Released, this, &AFPSCharacter::InputAimingReleased);
	
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AFPSCharacter::InputReload);
}

#pragma region NetWorking

void AFPSCharacter::ServerLowSpeedWalk_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

bool AFPSCharacter::ServerLowSpeedWalk_Validate()
{
	return true;
}

void AFPSCharacter::ServerNormalSpeedWalk_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

bool AFPSCharacter::ServerNormalSpeedWalk_Validate()
{
	return true;
}

void AFPSCharacter::ServerFireRifleWeapon_Implementation(FVector CameraVector, FRotator CameraRotation, bool IsMoving)
{
	if (ServerPrimaryWeapon)
	{
		ServerPrimaryWeapon->MultiShooting();

		ServerPrimaryWeapon->ClipCurrentAmmo -= 1;

		MultiShooting();

		ClientUpdateAmmoUI(ServerPrimaryWeapon->ClipCurrentAmmo, ServerPrimaryWeapon->GunCurrentAmmo);
	}

	IsFiring = true;
	RifleLineTrace(CameraVector, CameraRotation, IsMoving);
}

bool AFPSCharacter::ServerFireRifleWeapon_Validate(FVector CameraVector, FRotator CameraRotation, bool IsMoving)
{
	return true;
}

void AFPSCharacter::ServerFireSniperWeapon_Implementation(FVector CameraVector, FRotator CameraRotation, bool IsMoving)
{
	if (ServerPrimaryWeapon)
	{
		ServerPrimaryWeapon->MultiShooting();

		ServerPrimaryWeapon->ClipCurrentAmmo -= 1;

		MultiShooting();

		ClientUpdateAmmoUI(ServerPrimaryWeapon->ClipCurrentAmmo, ServerPrimaryWeapon->GunCurrentAmmo);
	}

	if (ClientPrimaryWeapon)
	{
		FLatentActionInfo ActionInfo;
		ActionInfo.CallbackTarget = this;
		ActionInfo.ExecutionFunction = TEXT("DelaySniperShootCallBack");
		ActionInfo.UUID = FMath::Rand();
		ActionInfo.Linkage = 0;
		UKismetSystemLibrary::Delay(this, ClientPrimaryWeapon->ClientArmsFireAnimMontage->GetPlayLength(), ActionInfo);
	}

	IsFiring = true;
	SniperLineTrace(CameraVector, CameraRotation, IsMoving);
}

bool AFPSCharacter::ServerFireSniperWeapon_Validate(FVector CameraVector, FRotator CameraRotation, bool IsMoving)
{
	return true;
}

void AFPSCharacter::ServerFirePistolWeapon_Implementation(FVector CameraVector, FRotator CameraRotation, bool IsMoving)
{
	if (ServerSecondaryWeapon)
	{
		FLatentActionInfo ActionInfo;
		ActionInfo.CallbackTarget = this;
		ActionInfo.ExecutionFunction = TEXT("DelaySpreadWeaponShootCallBack");
		ActionInfo.UUID = FMath::Rand();
		ActionInfo.Linkage = 0;
		UKismetSystemLibrary::Delay(this, ServerSecondaryWeapon->SpreadWeaponCallBackRate, ActionInfo);

		ServerSecondaryWeapon->MultiShooting();

		ServerSecondaryWeapon->ClipCurrentAmmo -= 1;

		//�ಥ
		MultiShooting();

		ClientUpdateAmmoUI(ServerSecondaryWeapon->ClipCurrentAmmo, ServerSecondaryWeapon->GunCurrentAmmo);
	}

	IsFiring = true;
	PistolLineTrace(CameraVector, CameraRotation, IsMoving);
}

bool AFPSCharacter::ServerFirePistolWeapon_Validate(FVector CameraVector, FRotator CameraRotation, bool IsMoving)
{
	return true;
}

void AFPSCharacter::MultiShooting_Implementation()
{
	AWeaponBaseServer* CurrentServerWeapon = GetCurrentServerTPBodysWeaponActor();
	if (ServerBodysAnimBP)
	{
		if (CurrentServerWeapon)
		{
			ServerBodysAnimBP->Montage_SetPlayRate(CurrentServerWeapon->ServerTPBodysShootAnimMotage, 1);
			ServerBodysAnimBP->Montage_Play(CurrentServerWeapon->ServerTPBodysShootAnimMotage);
		}
	}
}

bool AFPSCharacter::MultiShooting_Validate()
{
	return true;
}

void AFPSCharacter::MultiReloadAnimation_Implementation()
{
	AWeaponBaseServer* CurrentServerWeapon =  GetCurrentServerTPBodysWeaponActor();
	if (ServerBodysAnimBP)
	{
		if (CurrentServerWeapon)
		{
			ServerBodysAnimBP->Montage_Play(CurrentServerWeapon->ServerTPBodysReloadAnimMotage);
		}
	}
}

bool AFPSCharacter::MultiReloadAnimation_Validate()
{
	return true;
}

void AFPSCharacter::MultiSpawnBulletDecal_Implementation(FVector Location, FRotator Rotation)
{
	AWeaponBaseServer* CurrentServerWeapon = GetCurrentServerTPBodysWeaponActor();
	if (CurrentServerWeapon)
	{
		if (CurrentServerWeapon->BulletDecal)
		{
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CurrentServerWeapon->BulletDecal, FVector(8.0f, 8.0f, 8.0f), Location, Rotation, 10.0f);
			if (Decal)
			{
				Decal->SetFadeScreenSize(0.001f);
			}
		}
	}
}

bool AFPSCharacter::MultiSpawnBulletDecal_Validate(FVector Location, FRotator Rotation)
{
	return true;
}

void AFPSCharacter::MultiDeathMatch_Implementation()
{
	AWeaponBaseClient* CurrentClientWeapon = GetCurrentClientFPArmsWeaponActor();
	if (CurrentClientWeapon)
	{
		CurrentClientWeapon->Destroy();
	}
}

bool AFPSCharacter::MultiDeathMatch_Validate()
{
	return true;
}

void AFPSCharacter::ServerReloadPrimary_Implementation()
{
	if (ServerPrimaryWeapon)
	{
		if (ServerPrimaryWeapon->GunCurrentAmmo > 0 && ServerPrimaryWeapon->ClipCurrentAmmo < ServerPrimaryWeapon->MaxClipAmmo)
		{
			//�ͻ����ֱ۲��Ŷ���������������ಥ���������ݸ��£�UI����
			ClientReload();
			MultiReloadAnimation();
			IsReloading = true;
			if (ClientPrimaryWeapon)
			{
				FLatentActionInfo ActionInfo;
				ActionInfo.CallbackTarget = this;
				ActionInfo.ExecutionFunction = TEXT("DelayPlayArmReloadCallBack");
				ActionInfo.UUID = FMath::Rand();
				ActionInfo.Linkage = 0;
				UKismetSystemLibrary::Delay(this, ClientPrimaryWeapon->ClientArmsReloadAnimMontage->GetPlayLength(), ActionInfo);
			}
		}
	}
}

bool AFPSCharacter::ServerReloadPrimary_Validate()
{
	return true;
}

void AFPSCharacter::ServerReloadSecondary_Implementation()
{
	if (ServerSecondaryWeapon)
	{
		if (ServerSecondaryWeapon->GunCurrentAmmo > 0 && ServerSecondaryWeapon->ClipCurrentAmmo < ServerSecondaryWeapon->MaxClipAmmo)
		{
			//����
			ClientReload();
			MultiReloadAnimation();
			IsReloading = true;
			if (ClientSecondaryWeapon)
			{
				FLatentActionInfo ActionInfo;
				ActionInfo.CallbackTarget = this;
				ActionInfo.ExecutionFunction = TEXT("DelayPlayArmReloadCallBack");
				ActionInfo.Linkage = 0;
				ActionInfo.UUID = FMath::Rand();
				UKismetSystemLibrary::Delay(this, ClientSecondaryWeapon->ClientArmsReloadAnimMontage->GetPlayLength(), ActionInfo);
			}
		}
	}
}

bool AFPSCharacter::ServerReloadSecondary_Validate()
{
	return true;
}

void AFPSCharacter::ServerStopFire_Implementation()
{
	IsFiring = false;
}

bool AFPSCharacter::ServerStopFire_Validate()
{
	return true;
}

void AFPSCharacter::ServerSetAiming_Implementation(bool AimingState)
{
	IsAiming = AimingState;
}

bool AFPSCharacter::ServerSetAiming_Validate(bool AimingState)
{
	return true;
}


#pragma region Fire
void AFPSCharacter::ClientFire_Implementation()
{
	//�ͻ���Fire
	AWeaponBaseClient* CurrentClientWeapon = GetCurrentClientFPArmsWeaponActor();
	if (CurrentClientWeapon)
	{
		//����ǹ�Ķ���
		CurrentClientWeapon->PlayShootAnimation();

		//�ֱ۶���
		UAnimMontage* ClientArmsFireMontage = CurrentClientWeapon->ClientArmsFireAnimMontage;
		if (ClientArmsAnimBP && ClientArmsFireMontage)
		{
			ClientArmsAnimBP->Montage_SetPlayRate(ClientArmsFireMontage, 1);
			ClientArmsAnimBP->Montage_Play(ClientArmsFireMontage);
		}

		CurrentClientWeapon->DisplayWeaponEffect();

		FPC = Cast<AFPSPlayerController>(GetController());
		if (FPC)
		{
			FPC->PlayCameraShake(CurrentClientWeapon->FireCameraShake);

			FPC->DoCrosshairRecoil();
		}
	}
}

void AFPSCharacter::ClientUpdateAmmoUI_Implementation(int32 ClipCurrentAmmo, int32 GunCurrentAmmo)
{
	if (FPC)
	{
		FPC->UpdateAmmoUI(ClipCurrentAmmo, GunCurrentAmmo);
	}
}

void AFPSCharacter::ClientUpdateHealthUI_Implementation(float NewHeallth)
{
	if (FPC)
	{
		FPC->UpdateHealthUI(NewHeallth);
	}
}

void AFPSCharacter::ClientRecoil_Implementation()
{
	UCurveFloat* VerticalRecoilCurve = nullptr;
	UCurveFloat* HorizontalRecoilCurve = nullptr;
	if (ServerPrimaryWeapon)
	{
		VerticalRecoilCurve = ServerPrimaryWeapon->VerticalRecoilCurve;
		HorizontalRecoilCurve = ServerPrimaryWeapon->HorizontalRecoilCurve;
	}
	RecoilXCoordPerShoot += 0.1f;


	if (VerticalRecoilCurve)
	{
		NewVerticalRecoilAmount = VerticalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot);
	}

	if (HorizontalRecoilCurve)
	{
		NewHorizontalRecoilAmount = HorizontalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot);
	}

	VerticalRecoilAmount = NewVerticalRecoilAmount - OldVerticalRecoilAmount;
	HorizontalRecoilAmount = NewHorizontalRecoilAmount - OldHorizontalRecoilAmount;
	
	if (FPC)
	{
		FRotator ControllerRotator = FPC->GetControlRotation();
		FPC->SetControlRotation(FRotator(ControllerRotator.Pitch + VerticalRecoilAmount,
			ControllerRotator.Yaw + HorizontalRecoilAmount,
			ControllerRotator.Roll));
	}
	OldVerticalRecoilAmount = NewVerticalRecoilAmount;
	OldHorizontalRecoilAmount = NewHorizontalRecoilAmount;
}

void AFPSCharacter::ClientReload_Implementation()
{
	AWeaponBaseClient* CurrentClientWeapon = GetCurrentClientFPArmsWeaponActor();
	if (CurrentClientWeapon)
	{
		UAnimMontage* ClientArmsReloadMontage = CurrentClientWeapon->ClientArmsReloadAnimMontage;
		ClientArmsAnimBP->Montage_Play(ClientArmsReloadMontage);
		CurrentClientWeapon->PlayReloadAnimation();
	}
}

void AFPSCharacter::ClientAiming_Implementation()
{
	if (ArmsMesh && SniperScopeBPClass && ClientPrimaryWeapon && !IsReloading)
	{
		ArmsMesh->SetHiddenInGame(true);

		ClientPrimaryWeapon->SetActorHiddenInGame(true);

		if (PlayerCamera)
		{
			PlayerCamera->SetFieldOfView(ClientPrimaryWeapon->FieldOfAimingView);
		}

		if (FPC)
		{
			WidgetScope = CreateWidget<UUserWidget>(FPC, SniperScopeBPClass);
		}
		if (WidgetScope)
		{
			WidgetScope->AddToViewport();
		}
	}
}

void AFPSCharacter::ClientEndAiming_Implementation()
{
	if (!IsReloading)
	{
		if (ArmsMesh)
		{
			ArmsMesh->SetHiddenInGame(false);
		}

		if (ClientPrimaryWeapon)
		{
			ClientPrimaryWeapon->SetActorHiddenInGame(false);
			if (PlayerCamera)
			{
				PlayerCamera->SetFieldOfView(90.0f);
			}
		}

		if (WidgetScope)
		{
			WidgetScope->RemoveFromParent();
		}
	}
}

void AFPSCharacter::AutomaticFire()
{
	if (ServerPrimaryWeapon->ClipCurrentAmmo > 0)
	{
		//������
		if (UKismetMathLibrary::VSize(GetVelocity()) > 0.1f)
		{
			ServerFireRifleWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), true);
		}
		else
		{
			ServerFireRifleWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), false);
		}

		//�ͻ���
		ClientFire();
		//������
		ClientRecoil();
	}
	else
	{
		StopFirePrimary();
	}
}

void AFPSCharacter::ResetRecoil()
{
	NewVerticalRecoilAmount = 0.f;
	OldVerticalRecoilAmount = 0.f;
	VerticalRecoilAmount = 0.f;
	RecoilXCoordPerShoot = 0.f;
}

void AFPSCharacter::DelayPlayArmReloadCallBack()
{
	AWeaponBaseServer* CurrentServerWeapon = GetCurrentServerTPBodysWeaponActor();
	if (CurrentServerWeapon)
	{
		int32 GunCurrentAmmo = CurrentServerWeapon->GunCurrentAmmo;
		int32 ClipCurrentAmmo = CurrentServerWeapon->ClipCurrentAmmo;
		int32 const MaxClipAmmo = CurrentServerWeapon->MaxClipAmmo;
		IsReloading = false;
		if (MaxClipAmmo - ClipCurrentAmmo >= GunCurrentAmmo)
		{
			ClipCurrentAmmo += GunCurrentAmmo;
			GunCurrentAmmo = 0;
		}
		else
		{
			GunCurrentAmmo -= MaxClipAmmo - ClipCurrentAmmo;
			ClipCurrentAmmo = MaxClipAmmo;
		}
		CurrentServerWeapon->GunCurrentAmmo = GunCurrentAmmo;
		CurrentServerWeapon->ClipCurrentAmmo = ClipCurrentAmmo;
		//���µ�ҩ
		ClientUpdateAmmoUI(ClipCurrentAmmo, GunCurrentAmmo);
	}
}

void AFPSCharacter::FireWeaponPrimary()
{
	if (ServerPrimaryWeapon && ServerPrimaryWeapon->ClipCurrentAmmo > 0 && !IsReloading)
	{
		//������
		if (UKismetMathLibrary::VSize(GetVelocity()) > 0.1f)
		{
			ServerFireRifleWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), true);
		}
		else
		{
			ServerFireRifleWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), false);
		}
		//�ͻ���
		ClientFire();

		ClientRecoil();

		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ClipCurrentAmmo:%d"), ServerPrimaryWeapon->ClipCurrentAmmo));
		if (ServerPrimaryWeapon->IsAutomatic)
		{
			GetWorldTimerManager().SetTimer(AutomaticFireTimerHandle, this, &AFPSCharacter::AutomaticFire,
				ServerPrimaryWeapon->AutomaticFireRate, true);
		}
	}
}

void AFPSCharacter::StopFirePrimary()
{
	//����IsFiring����
	ServerStopFire();

	//��ʱ��
	GetWorldTimerManager().ClearTimer(AutomaticFireTimerHandle);

	//���ú�������ر���
	ResetRecoil();
}

void AFPSCharacter::RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	FVector EndLocation;
	TArray<AActor*> IgnoreArray;
	IgnoreArray.Add(this);
	FHitResult HitResult;

	if (ServerPrimaryWeapon)
	{
		if (IsMoving)
		{
			//x,y,zȫ�ּ�һ�����ƫ��
			FVector vector = CameraLocation + CameraRotation.Vector() * ServerPrimaryWeapon->BullentDistance;
			float RandomX = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
			float RandomY = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
			float RandomZ = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
			EndLocation = FVector(vector.X + RandomX, vector.Y + RandomY, vector.Z + RandomZ);
		}
		else
		{
			EndLocation = CameraLocation + (CameraRotation.Vector() * ServerPrimaryWeapon->BullentDistance);
		}
	}

	
	bool HitSucceed = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreArray, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green);
	if (HitSucceed)
	{
		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor : %s"), *HitResult.Actor->GetName()));
		AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(HitResult.Actor);
		
		if (FPSCharacter)
		{
			//����ң�Ӧ���˺�
			DamagePlayer(HitResult.PhysMaterial.Get(), HitResult.Actor.Get(), CameraLocation, HitResult);
		}
		else
		{
			//��ǽ�ڣ����ɵ�������
			FRotator XRotator = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
			MultiSpawnBulletDecal(HitResult.Location, XRotator);
		}
	}
}

void AFPSCharacter::FireWeaponSniper()
{
	if (ServerPrimaryWeapon && ServerPrimaryWeapon->ClipCurrentAmmo > 0 && !IsReloading && !IsFiring)
	{
		//������
		if (UKismetMathLibrary::VSize(GetVelocity()) > 0.1f)
		{
			ServerFireSniperWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), true);
		}
		else
		{
			ServerFireSniperWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), false);
		}
		//�ͻ���
		ClientFire();
	}
}

void AFPSCharacter::StopFireSniper()
{

}

void AFPSCharacter::SniperLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	FVector EndLocation;
	TArray<AActor*> IgnoreArray;
	IgnoreArray.Add(this);
	FHitResult HitResult;

	if (ServerPrimaryWeapon)
	{
		//�Ƿ���׼
		if (IsAiming)
		{
			if (IsMoving)
			{
				//x,y,zȫ�ּ�һ�����ƫ��
				FVector vector = CameraLocation + CameraRotation.Vector() * ServerPrimaryWeapon->BullentDistance;
				float RandomX = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
				float RandomY = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
				float RandomZ = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
				EndLocation = FVector(vector.X + RandomX, vector.Y + RandomY, vector.Z + RandomZ);
			}
			else
			{
				EndLocation = CameraLocation + (CameraRotation.Vector() * ServerPrimaryWeapon->BullentDistance);
			}
			ClientEndAiming();
		}
		else
		{
			//x,y,zȫ�ּ�һ�����ƫ��
			FVector vector = CameraLocation + CameraRotation.Vector() * ServerPrimaryWeapon->BullentDistance;
			float RandomX = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
			float RandomY = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
			float RandomZ = UKismetMathLibrary::RandomFloatInRange(-ServerPrimaryWeapon->MovingFireRandomRange, ServerPrimaryWeapon->MovingFireRandomRange);
			EndLocation = FVector(vector.X + RandomX, vector.Y + RandomY, vector.Z + RandomZ);
		}
	}

	bool HitSucceed = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreArray, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green);
	if (HitSucceed)
	{
		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor : %s"), *HitResult.Actor->GetName()));
		AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(HitResult.Actor);

		if (FPSCharacter)
		{
			//����ң�Ӧ���˺�
			DamagePlayer(HitResult.PhysMaterial.Get(), HitResult.Actor.Get(), CameraLocation, HitResult);
		}
		else
		{
			//��ǽ�ڣ����ɵ�������
			FRotator XRotator = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
			MultiSpawnBulletDecal(HitResult.Location, XRotator);
		}
	}
}

void AFPSCharacter::DelaySniperShootCallBack()
{
	IsFiring = false;
}

void AFPSCharacter::FireWeaponSecondary()
{
	if (ServerSecondaryWeapon && ServerSecondaryWeapon->ClipCurrentAmmo > 0 && !IsReloading)
	{
		//������
		if (UKismetMathLibrary::VSize(GetVelocity()) > 0.1f)
		{
			ServerFirePistolWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), true);
		}
		else
		{
			ServerFirePistolWeapon(PlayerCamera->GetComponentLocation(), PlayerCamera->GetComponentRotation(), false);
		}
		//�ͻ���
		ClientFire();
	}
}

void AFPSCharacter::StopFireSecondary()
{
	//����IsFiring����
	ServerStopFire();
}

void AFPSCharacter::PistolLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	FVector EndLocation;
	TArray<AActor*> IgnoreArray;
	FVector CameraForwardVector;
	IgnoreArray.Add(this);
	FHitResult HitResult;

	if (ServerSecondaryWeapon)
	{
		if (IsMoving)
		{
			FRotator Rotator;
			Rotator.Roll = CameraRotation.Roll;
			Rotator.Pitch = CameraRotation.Pitch + UKismetMathLibrary::RandomFloatInRange(PistolSpreadMin, PistolSpreadMax);
			Rotator.Yaw = CameraRotation.Yaw + UKismetMathLibrary::RandomFloatInRange(PistolSpreadMin, PistolSpreadMax);
			CameraForwardVector = UKismetMathLibrary::GetForwardVector(Rotator);
			//x,y,zȫ�ּ�һ�����ƫ��
			FVector vector = CameraLocation + CameraForwardVector * ServerSecondaryWeapon->BullentDistance;
			float RandomX = UKismetMathLibrary::RandomFloatInRange(-ServerSecondaryWeapon->MovingFireRandomRange, ServerSecondaryWeapon->MovingFireRandomRange);
			float RandomY = UKismetMathLibrary::RandomFloatInRange(-ServerSecondaryWeapon->MovingFireRandomRange, ServerSecondaryWeapon->MovingFireRandomRange);
			float RandomZ = UKismetMathLibrary::RandomFloatInRange(-ServerSecondaryWeapon->MovingFireRandomRange, ServerSecondaryWeapon->MovingFireRandomRange);
			EndLocation = FVector(vector.X + RandomX, vector.Y + RandomY, vector.Z + RandomZ);
		}
		else
		{
			//��ת��һ�����ƫ��
			FRotator Rotator;
			Rotator.Roll = CameraRotation.Roll;
			Rotator.Pitch = CameraRotation.Pitch + UKismetMathLibrary::RandomFloatInRange(PistolSpreadMin, PistolSpreadMax);
			Rotator.Yaw = CameraRotation.Yaw + UKismetMathLibrary::RandomFloatInRange(PistolSpreadMin, PistolSpreadMax);
			CameraForwardVector = UKismetMathLibrary::GetForwardVector(Rotator);
			EndLocation = CameraLocation + CameraForwardVector * ServerSecondaryWeapon->BullentDistance;
		}
	}


	bool HitSucceed = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreArray, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green);
	PistolSpreadMax += ServerSecondaryWeapon->SpreadWeaponMaxIndex;
	PistolSpreadMin -= ServerSecondaryWeapon->SpreadWeaponMinIndex;
	if (HitSucceed)
	{
		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor : %s"), *HitResult.Actor->GetName()));
		AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(HitResult.Actor);

		if (FPSCharacter)
		{
			//����ң�Ӧ���˺�
			DamagePlayer(HitResult.PhysMaterial.Get(), HitResult.Actor.Get(), CameraLocation, HitResult);
		}
		else
		{
			//��ǽ�ڣ����ɵ�������
			FRotator XRotator = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
			MultiSpawnBulletDecal(HitResult.Location, XRotator);
		}
	}
}

void AFPSCharacter::DelaySpreadWeaponShootCallBack()
{
	PistolSpreadMin = 0.0f;
	PistolSpreadMax = 0.0f;
}

void AFPSCharacter::DamagePlayer(UPhysicalMaterial* PhysicalMaterial, AActor* DamagedActor, const FVector& HitFromDirection, const FHitResult& HitInfo)
{
	//�����ͬ��λ���յ��˺���ͬ
	AWeaponBaseServer* CurrentServerWeapon = GetCurrentServerTPBodysWeaponActor();

	if (CurrentServerWeapon)
	{
		switch (PhysicalMaterial->SurfaceType)
		{
		case EPhysicalSurface::SurfaceType1:
		{
			//Head
			UGameplayStatics::ApplyPointDamage(DamagedActor, CurrentServerWeapon->BaseDamage * 4, HitFromDirection, HitInfo, GetController(), this, UDamageType::StaticClass());
		}
		break;
		case EPhysicalSurface::SurfaceType2:
		{
			//Body
			UGameplayStatics::ApplyPointDamage(DamagedActor, CurrentServerWeapon->BaseDamage * 1, HitFromDirection, HitInfo, GetController(), this, UDamageType::StaticClass());
		}
		break;
		case EPhysicalSurface::SurfaceType3:
		{
			//Arm
			UGameplayStatics::ApplyPointDamage(DamagedActor, CurrentServerWeapon->BaseDamage * 0.8f, HitFromDirection, HitInfo, GetController(), this, UDamageType::StaticClass());
		}
		break;
		case EPhysicalSurface::SurfaceType4:
		{
			//Leg
			UGameplayStatics::ApplyPointDamage(DamagedActor, CurrentServerWeapon->BaseDamage * 0.7f, HitFromDirection, HitInfo, GetController(), this, UDamageType::StaticClass());
		}
		break;
		default:
			break;
		}
	}
}

void AFPSCharacter::OnHit(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	
	//�ı������ɫ
	//ClientUpdateCrosshair();
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("PlayerName: %s Health: %f"),*GetName(), Health));
	//1.�ͻ���RPC 2.��ͻ���Playcontroller��һ������
	ClientUpdateHealthUI(Health);
	if (Health == 0)
	{
		Death = true;
		AWeaponBaseClient* CurrentClientWeapon = GetCurrentClientFPArmsWeaponActor();
		AWeaponBaseServer* CurrentServerWeapon = GetCurrentServerTPBodysWeaponActor();
		//������������
		DeathMatch(DamageCauser);
		//�ͻ�����������
		MultiDeathMatch();

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(3.0f);
		if (CurrentClientWeapon)
		{
			CurrentClientWeapon->Destroy();
		}
		if (CurrentServerWeapon)
		{
			CurrentServerWeapon->SetLifeSpan(3.0f);
		}
	}
}

void AFPSCharacter::DeathMatch(AActor* DamageActor)
{
	AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->DeathMatch(DamageActor);
	}
}

//����
void AFPSCharacter::ClientUpdateCrosshair_Implementation()
{
	AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->UpdateCrosshair();
	}
}

void AFPSCharacter::ClientEquipFPArmsPrimary_Implementation()
{
	if (ServerPrimaryWeapon)
	{
		if (ClientPrimaryWeapon)
		{

		}
		else
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ClientPrimaryWeapon = GetWorld()->SpawnActor<AWeaponBaseClient>(ServerPrimaryWeapon->ClientWeaponBaseBPClass, GetActorTransform(), SpawnInfo);	
			//����Ҫ��
			FName WeaponSocketName = TEXT("WeaponSocket");
			if (ActiveWeapon == EWeaponType::M4A1)
			{
				WeaponSocketName = TEXT("M4A1_Socket");
			}
			if (ActiveWeapon == EWeaponType::SVD)
			{
				WeaponSocketName = TEXT("AWP_Socket");
			}
			ClientPrimaryWeapon->K2_AttachToComponent(ArmsMesh, WeaponSocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			
			ClientUpdateAmmoUI(ServerPrimaryWeapon->ClipCurrentAmmo, ServerPrimaryWeapon->GunCurrentAmmo);

			//�ֱ۶������
			if (ClientPrimaryWeapon)
			{
				UpdateFPArmsBlendPose(ClientPrimaryWeapon->FPArmsBlendPose);
				//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("WeaponSocketName:%s"), *WeaponSocketName.ToString()));
			}
		}
	}
}

void AFPSCharacter::ClientEquipFPArmsSecondary_Implementation()
{
	if (ServerSecondaryWeapon)
	{
		if (ClientSecondaryWeapon)
		{

		}
		else
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ClientSecondaryWeapon = GetWorld()->SpawnActor<AWeaponBaseClient>(ServerSecondaryWeapon->ClientWeaponBaseBPClass, GetActorTransform(), SpawnInfo);
			//����Ҫ��
			FName WeaponSocketName = TEXT("WeaponSocket");
			ClientSecondaryWeapon->K2_AttachToComponent(ArmsMesh, WeaponSocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

			ClientUpdateAmmoUI(ServerSecondaryWeapon->ClipCurrentAmmo, ServerSecondaryWeapon->GunCurrentAmmo);

			//�ֱ۶������
			if (ClientSecondaryWeapon)
			{
				UpdateFPArmsBlendPose(ClientSecondaryWeapon->FPArmsBlendPose);
				//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("WeaponSocketName:%s"), *WeaponSocketName.ToString()));
			}
		}
	}
}
#pragma endregion
#pragma endregion

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, IsFiring);
	DOREPLIFETIME(AFPSCharacter, IsReloading);
	DOREPLIFETIME(AFPSCharacter, ActiveWeapon);
	DOREPLIFETIME(AFPSCharacter, IsAiming);
	DOREPLIFETIME(AFPSCharacter, Death);
}