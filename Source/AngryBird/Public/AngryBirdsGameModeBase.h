// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AngryBirdsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRD_API AAngryBirdsGameModeBase : public AGameModeBase
{
    GENERATED_BODY()
public:
    AAngryBirdsGameModeBase();
    UFUNCTION()
    void OnTargetDestroyed();

protected:
    void BeginPlay() override;
private:
    int32 Score = 0;
    int32 TotalTargets = 0;
    FTimerHandle GameTimerHandle;


    UFUNCTION()
    void EndGame(bool bIsWin);
    UFUNCTION()
    void OnTimerComplete();
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Rules")
    float GameTime = 60.0f;
    UFUNCTION(BlueprintCallable, Category = "Game")
    int32 GetCurrentScore() const;
    UFUNCTION(BlueprintCallable, Category = "Game")
    float GetTimeRemaining() const;
};