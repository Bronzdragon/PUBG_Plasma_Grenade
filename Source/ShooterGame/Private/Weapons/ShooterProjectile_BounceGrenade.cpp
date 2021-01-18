// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterProjectile_BounceGrenade.h"

AShooterProjectile_BounceGrenade::AShooterProjectile_BounceGrenade(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    MovementComp->bShouldBounce = true;
	MovementComp->Bounciness = 0.9f;
	MovementComp->BounceVelocityStopSimulatingThreshold = 0.001f;
	MovementComp->ProjectileGravityScale = 1.0f;

	CollisionComp->OnComponentHit.AddDynamic(this, &AShooterProjectile_BounceGrenade::OnOverlapBegin);
	MovementComp->OnProjectileBounce.AddDynamic(this, &AShooterProjectile_BounceGrenade::OnBounce);
}

void AShooterProjectile_BounceGrenade::BeginPlay() {
	Super::BeginPlay();
	if (Role == ROLE_Authority && !bExploded) {
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &AShooterProjectile_BounceGrenade::OnFuseEnd, fuseLength);
	}
}

void AShooterProjectile_BounceGrenade::OnImpact(const FHitResult& HitResult) {
	// Parent implementation is ignored on purpose.
}

void AShooterProjectile_BounceGrenade::OnOverlapBegin(UPrimitiveComponent *newComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector impact, const FHitResult &SweepResult) {
	if (bExploded || bStuckToTarget) return;

	APawn* overlappingPawn = Cast<APawn>(OtherActor);
	APawn* myPawn = MyController.IsValid() ? MyController->GetPawn() : nullptr;
	
	if (!overlappingPawn || !myPawn) return;
	if (ignoreOwnPawn && myPawn == overlappingPawn) return;

	ServerStickToTarget(FHitResult{ overlappingPawn, OtherComp, SweepResult.Location, -SweepResult.Normal });
}

void AShooterProjectile_BounceGrenade::OnFuseEnd() {
	FHitResult result{ this, CollisionComp, GetActorLocation(), FVector{} };

	if (HasAuthority() && !bExploded) {
		Explode(result, false);
		DisableAndDestroy();
	}
}

void AShooterProjectile_BounceGrenade::OnBounce(const FHitResult & HitResult, const FVector &Vector) {
	APawn* myPawn = MyController.IsValid() ? MyController->GetPawn() : nullptr;
	if (ignoreOwnPawn && myPawn && myPawn != HitResult.GetActor()) {
		// After the first bounce, the grenade should get sticky.
		ignoreOwnPawn = false;
	}
}

void AShooterProjectile_BounceGrenade::Explode_Implementation(const FHitResult& Impact, bool shouldOffset) {
	Super::Explode(Impact, shouldOffset);
}

bool AShooterProjectile_BounceGrenade::ServerStickToTarget_Validate(const FHitResult& HitResult) {
	return true;
}

void AShooterProjectile_BounceGrenade::ServerStickToTarget_Implementation(const FHitResult& HitResult) {
	if (Cast<APawn>(HitResult.GetActor()) == nullptr || bExploded && bStuckToTarget) return;

	bStuckToTarget = true;
	StickTarget = HitResult.GetActor();

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnRep_StickToTarget();
}



void AShooterProjectile_BounceGrenade::OnRep_StickToTarget() {
	if (!StickTarget.IsValid()) return;

	MovementComp->StopSimulating(FHitResult{ StickTarget.Get(), nullptr, GetActorLocation(), FVector{} });
	this->AttachToActor(StickTarget.Get(), FAttachmentTransformRules{ EAttachmentRule::KeepWorld, true });
}

void AShooterProjectile_BounceGrenade::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterProjectile_BounceGrenade, bStuckToTarget);
	DOREPLIFETIME(AShooterProjectile_BounceGrenade, StickTarget);
}