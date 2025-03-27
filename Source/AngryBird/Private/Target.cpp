// Fill out your copyright notice in the Description page of Project Settings.

#include "Target.h"
#include "Components/SceneComponent.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

   // Root = CreateDefaultSubobject<USceneComponent>(TEXT("Target Root"));

    geometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("geometryCollection"));
    geometryCollection->SetSimulatePhysics(true);
    geometryCollection->SetNotifyRigidBodyCollision(true);
    geometryCollection->SetEnableDamageFromCollision(true);
    geometryCollection->EnableClustering = true;
    geometryCollection->bNotifyBreaks = true;
    geometryCollection->ClusterGroupIndex = 1;
    geometryCollection->DamageThreshold.Empty();
    geometryCollection->DamageThreshold.Add(1.0f);
    geometryCollection->BodyInstance.SetCollisionProfileName(TEXT("PhysicsActor"));
    SetRootComponent(geometryCollection);

   // geometryCollection->OnComponentHit.AddDynamic(this, &ATarget::OnHit);
   // geometryCollection->SetupAttachment(Root);


}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATarget::ApplyDestructionField(FVector ImpactLocation)
{
    UE_LOG(LogTemp, Warning, TEXT("Applying destruction field at location: %s"), *ImpactLocation.ToString());

    // Création du Field System
    UFieldSystemComponent* FieldSystem = NewObject<UFieldSystemComponent>(this);
    FieldSystem->AttachToComponent(geometryCollection, FAttachmentTransformRules::KeepWorldTransform);
    FieldSystem->RegisterComponent();

    // Création d'un masque radial pour appliquer la destruction
    URadialIntMask* RadialMask = NewObject<URadialIntMask>();
    RadialMask->Radius = 300.0f;  // Zone d'effet
    RadialMask->Position = ImpactLocation;
    RadialMask->InteriorValue = 1;  // Fracture les morceaux à l'intérieur
    RadialMask->ExteriorValue = 0;  // Ne touche pas l'extérieur

    // Appliquer le champ de destruction
    FieldSystem->ApplyPhysicsField(true, EFieldPhysicsType::Field_ExternalClusterStrain, nullptr, RadialMask);

    UE_LOG(LogTemp, Warning, TEXT("RadialIntMask applied with Radius: %f"), RadialMask->Radius);


    //UFieldSystemComponent* FieldSystem = NewObject<UFieldSystemComponent>(this);
    //FieldSystem->AttachToComponent(geometryCollection, FAttachmentTransformRules::KeepWorldTransform);
    //FieldSystem->RegisterComponent();

    //// Crée une force radiale qui va causer la destruction
    //URadialFalloff* RadialFalloff = NewObject<URadialFalloff>();
    //RadialFalloff->Magnitude = 2000.0f; // Force appliquée sur les morceaux
    //RadialFalloff->Radius = 250.0f;    // Zone d'effet
    //RadialFalloff->Position = ImpactLocation;

    //// Applique la force de destruction
    //FieldSystem->ApplyPhysicsField(true, EFieldPhysicsType::Field_ExternalClusterStrain, nullptr, RadialFalloff);
}

void ATarget::OnBreak(const FChaosBreakEvent& BreakEvent)
{
    UE_LOG(LogTemp, Warning, TEXT("Target fractured!"));

}

void ATarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->ActorHasTag(TEXT("Floor")))
        return;
  /*  float ImpactForce = NormalImpulse.Size();
    float DestructionThreshold = 500.0f;

    if (ImpactForce > DestructionThreshold)*/
    UE_LOG(LogTemp, Warning, TEXT("Hit detected with %s"), *OtherActor->GetName());

        ApplyDestructionField(Hit.ImpactPoint);
}

