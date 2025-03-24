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
protected:
	void BeginPlay() override;
};
