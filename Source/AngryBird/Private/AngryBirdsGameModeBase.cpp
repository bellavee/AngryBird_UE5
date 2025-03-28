// Fill out your copyright notice in the Description page of Project Settings.


#include "AngryBirdsGameModeBase.h"
#include "GameFramework/Actor.h"
#include "SlingShotPawn.h"

AAngryBirdsGameModeBase::AAngryBirdsGameModeBase()
{
	DefaultPawnClass = ASlingShotPawn::StaticClass();
}

void AAngryBirdsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("GameMode Actif : %s"), *GetName());

}
