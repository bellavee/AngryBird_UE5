// Fill out your copyright notice in the Description page of Project Settings.

#include "Target.h"
#include "Components/SceneComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ATarget::ATarget() {
	PrimaryActorTick.bCanEverTick = true;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetSimulatePhysics(true);
	GeometryCollection->SetNotifyRigidBodyCollision(true);
	GeometryCollection->SetEnableDamageFromCollision(true);
	GeometryCollection->EnableClustering = true;
	GeometryCollection->bNotifyBreaks = true;
	GeometryCollection->ClusterGroupIndex = 0;
	GeometryCollection->BodyInstance.SetCollisionProfileName(TEXT("PhysicsActor"));
	SetRootComponent(GeometryCollection);

}

void ATarget::BeginPlay() {
	Super::BeginPlay();
}


void ATarget::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
