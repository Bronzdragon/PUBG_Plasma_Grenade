// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterProjectile.h"
#include "ShooterProjectile_BounceGrenade.generated.h"


/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterProjectile_BounceGrenade : public AShooterProjectile
{
	GENERATED_BODY()

	bool ignoreOwnPawn = true;
	
	virtual void BeginPlay() override;

	/** handle hit with pawn */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent *newComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector impact, const FHitResult &SweepResult);

public:
	AShooterProjectile_BounceGrenade(const FObjectInitializer&);

	UPROPERTY(Transient, Replicated, BlueprintReadWrite, Category = "Grenade")
	bool bStuckToTarget = false;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_StickToTarget, BlueprintReadWrite, Category = "Grenade")
	TWeakObjectPtr<AActor> StickTarget;

	/** [client] explosion happened */
	UFUNCTION()
	void OnRep_StickToTarget();

	UPROPERTY(EditAnywhere, Category="Grenade")
	float fuseLength = 3.0f;
	
	/** handle hit */
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	/** handle fuse */
	UFUNCTION()
	void OnFuseEnd();

	/** handle bounce*/
	UFUNCTION()
	void OnBounce(const FHitResult & HitResult, const FVector &Vector);

	UFUNCTION(BlueprintNativeEvent)
	void Explode(const FHitResult& Impact, bool shouldOffset);
	virtual void Explode_Implementation(const FHitResult& Impact, bool shouldOffset);

	UFUNCTION(Server, WithValidation, Reliable)
	void ServerStickToTarget(const FHitResult& HitResult);


};
