// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeapon_Projectile.h"
#include "ShooterWeapon_GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class AShooterWeapon_GrenadeLauncher : public AShooterWeapon_Projectile
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector getMuzzleLocation() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float grenadeOffset = 100.0f;
	
};
