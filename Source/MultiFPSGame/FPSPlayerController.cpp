// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

void AFPSPlayerController::PlayCameraShake(TSubclassOf<class UCameraShakeBase> CameraShakeBase)
{
	ClientPlayCameraShake(CameraShakeBase);
}