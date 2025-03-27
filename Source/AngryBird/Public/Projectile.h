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

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Launch(FVector LaunchVelocity);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	float ImpactForceBase = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	float ForceRadius = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	float DirectedForceMultiplier = 4.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactEffect;

private:
	float ImpactForceMultiplier = 1.0f;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
			   const FHitResult& Hit);

};
