#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "Projectile.generated.h"


UCLASS()

class ANGRYBIRD_API AProjectile : public AActor {
	GENERATED_BODY()

public:
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Launch(FVector LaunchVelocity);
};
