#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AProjectile::BeginPlay() {
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
						const FHitResult& Hit) {
	UGeometryCollectionComponent* GeometryCollection = Cast<UGeometryCollectionComponent>(OtherComp);
	
	if (GeometryCollection) {
		float ForceMagnitude = ImpactForceMultiplier * ImpactForceBase;
		FVector ForceDirection = ProjectileMovement->Velocity.GetSafeNormal();
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%f"), ForceMagnitude));

		GeometryCollection->AddRadialImpulse(
			Hit.Location,       // Center
			ForceRadius,        // Radius
			ForceMagnitude,     // Strength
			ERadialImpulseFalloff::RIF_Linear,
			true                // Apply impulse
		);

		FVector Impulse = ForceDirection * ForceMagnitude * DirectedForceMultiplier;
		
		GeometryCollection->AddImpulseAtLocation(
			Impulse,
			Hit.Location
		);

		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("%f, %f"), Impulse.X, Impulse.Y));
		
		if (ImpactEffect) {
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactEffect,
				Hit.Location,
				ForceDirection.Rotation()
			);
		}
		
		Destroy();
	}
}

void AProjectile::Launch(FVector LaunchVelocity) {
	ProjectileMovement->Velocity = LaunchVelocity;
}

