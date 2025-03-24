// Fill out your copyright notice in the Description page of Project Settings.


#include "SlingShotPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASlingShotPawn::ASlingShotPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("SlingShotMesh");
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Root = CreateDefaultSubobject<USceneComponent>("Root");

	SetRootComponent(Root);
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300;
	Camera->SetupAttachment(SpringArm);
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASlingShotPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlingShotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlingShotPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

