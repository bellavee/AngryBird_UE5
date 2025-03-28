// Fill out your copyright notice in the Description page of Project Settings.


#include "AngryBirdsGameModeBase.h"
#include "GameFramework/Actor.h"
#include "SlingShotPawn.h"
#include "Target.h"
#include "Kismet/GameplayStatics.h"

AAngryBirdsGameModeBase::AAngryBirdsGameModeBase()
{
	DefaultPawnClass = ASlingShotPawn::StaticClass();
}

void AAngryBirdsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    TArray<AActor*> Targets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATarget::StaticClass(), Targets);

    TotalTargets = Targets.Num();
    UE_LOG(LogTemp, Warning, TEXT("Nombre total de cibles d�tect�es: %d"), TotalTargets);
    /*for (AActor* TargetActor : Targets) {
        ATarget* Target = Cast<ATarget>(TargetActor);
        if (Target)
            Target->OnTargetDestroyed.AddDynamic(this, &AAngryBirdsGameModeBase::OnTargetDestroyed);
    }*/

    GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &AAngryBirdsGameModeBase::OnTimerComplete, GameTime, false);
}

void AAngryBirdsGameModeBase::OnTargetDestroyed()
{
    Score++;
    if (Score >= TotalTargets)
        EndGame(true);
}

void AAngryBirdsGameModeBase::EndGame(bool bIsWin)
{
    GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);

    if (bIsWin) {
        UE_LOG(LogTemp, Warning, TEXT("you win"));
        GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("you loose"));
        UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
    }
}

void AAngryBirdsGameModeBase::OnTimerComplete()
{
    EndGame(false);
    UE_LOG(LogTemp, Warning, TEXT("GameMode Actif : %s"), *GetName());

}
