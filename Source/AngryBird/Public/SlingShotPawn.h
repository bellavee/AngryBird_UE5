#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SlingShotPawn.generated.h"

UCLASS()
class ANGRYBIRD_API ASlingShotPawn : public APawn {
	GENERATED_BODY()

public:
	ASlingShotPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* LaunchPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slingshot")
	TSubclassOf<class AProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slingshot")
	float LaunchMultiplier = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slingshot")
	float MaxPullDistance = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Slingshot")
	bool bValidPullDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* PullAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* ReleaseAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* SlingshotMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory")
	float TrajectoryTimeStep = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory")
	float TrajectoryDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory")
	float TrajectoryPointSize = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Raycast")
	UStaticMeshComponent* PullPlanMesh;

	UPROPERTY(EditAnywhere, Category = "Slingshot")
	float PullLimitY = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Slingshot")
	float PullLimitZ = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Slingshot")
	float SpawnDelay = 3.0f;

	UPROPERTY(VisibleAnywhere, Category = "Slingshot")
	bool bIsInSpawnDelay = false;
	
private:
	bool bIsPulling;
	FVector PullPosition;
	class AProjectile* PreviewProjectile;

	void SpawnPreviewProjectile();
	void PullSlingshot(const FInputActionValue& Value);
	void ReleaseSlingshot(const FInputActionValue& Value);
	FVector CalculateLaunchVelocity();
	void DrawTrajectory(FColor TrajectoryColor = FColor::Yellow);
	void OnSpawnDelayComplete();
};