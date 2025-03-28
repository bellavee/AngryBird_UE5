// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

UCLASS()
class ANGRYBIRD_API ATarget : public AActor
{
	GENERATED_BODY()
	
public:	
	ATarget();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGeometryCollectionComponent* GeometryCollection;
	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDestroyed = false;

	UFUNCTION()
	void OnBreak(const FChaosBreakEvent& BreakEvent);*/

};
