// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon_GrenadeLauncher.h"

FVector AShooterWeapon_GrenadeLauncher::getMuzzleLocation() const {
	return GetActorLocation() + (GetActorRightVector() * grenadeOffset);
}