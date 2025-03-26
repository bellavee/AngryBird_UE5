#include "SlingShotPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASlingShotPawn::ASlingShotPawn() {
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlingShotMesh"));
	MeshComponent->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f)); 
	SpringArm->SetRelativeLocation(FVector(-100.0f, 0.0f, 150.0f)); 
	SpringArm->bEnableCameraLag = true; 
	SpringArm->CameraLagSpeed = 5.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->FieldOfView = 90.0f;

	LaunchPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LaunchPoint"));
	LaunchPoint->SetupAttachment(RootComponent);
	LaunchPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	PullPlanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PullPlan"));
	PullPlanMesh->SetupAttachment(RootComponent);
	
	bIsPulling = false;
	PreviewProjectile = nullptr;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASlingShotPawn::BeginPlay() {
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController()) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer())) {
			Subsystem->AddMappingContext(SlingshotMappingContext, 0);
		}
		PC->bShowMouseCursor = true;
	}

	SpawnPreviewProjectile();
}

void ASlingShotPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (bIsPulling) {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC) {
            float MouseX, MouseY;
            PC->GetMousePosition(MouseX, MouseY);
            FVector WorldPos, WorldDir;
            PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldPos, WorldDir);

            FVector PlaneNormal = PullPlanMesh->GetForwardVector();
            FVector PlaneOrigin = PullPlanMesh->GetComponentLocation();
            
            float Denominator = FVector::DotProduct(PlaneNormal, WorldDir);
            if (FMath::Abs(Denominator) > SMALL_NUMBER) {
                float t = FVector::DotProduct(PlaneNormal, PlaneOrigin - WorldPos) / Denominator;
                if (t >= 0) {
                    FVector PotentialPullPosition = WorldPos + WorldDir * t;
                    FVector PullVector = PotentialPullPosition - LaunchPoint->GetComponentLocation();
                    FVector LocalPullVector = GetActorTransform().InverseTransformVector(PullVector);
                    
                	bValidPullDirection = (LocalPullVector.X < 0) && 
										  (LocalPullVector.Y >= -20) && 
										  (LocalPullVector.Y <= 20);
                    PullPosition = PotentialPullPosition;

                	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("%f, %f"), PullVector.X, PullVector.Y));
                    
                    float PullDistance = PullVector.Size();
                    if (PullDistance > MaxPullDistance) {
                        PullPosition = LaunchPoint->GetComponentLocation() + (PullVector.GetSafeNormal() * MaxPullDistance);
                    }
                    
                    if (PreviewProjectile) {
                        PreviewProjectile->SetActorLocation(PullPosition);
                    }

                    FColor TrajectoryColor = bValidPullDirection ? FColor::Yellow : FColor::Red;
                    DrawTrajectory(TrajectoryColor);
                }
            }
        }
    }
}


void ASlingShotPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(PullAction, ETriggerEvent::Triggered, this, &ASlingShotPawn::PullSlingshot);
		EnhancedInputComponent->BindAction(ReleaseAction, ETriggerEvent::Completed, this, &ASlingShotPawn::ReleaseSlingshot);
	}
}

void ASlingShotPawn::SpawnPreviewProjectile() {
	if (PreviewProjectile) {
		PreviewProjectile->Destroy();
		PreviewProjectile = nullptr;
	}

	if (ProjectileClass) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		PreviewProjectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			LaunchPoint->GetComponentLocation(),
			FRotator::ZeroRotator,
			SpawnParams
		);
        
		if (PreviewProjectile) {
			PreviewProjectile->ProjectileMovement->SetActive(false);
		}
	}
}

void ASlingShotPawn::PullSlingshot(const FInputActionValue& Value) {
	if (bIsInSpawnDelay) {
		return;
	}
	
	bIsPulling = Value.Get<bool>();
}

void ASlingShotPawn::ReleaseSlingshot(const FInputActionValue& Value) {
	if (!bIsPulling || !ProjectileClass || !PreviewProjectile) {
		return;
	}

	bIsPulling = false;
	bIsInSpawnDelay = true;

	if (bValidPullDirection) {
		FVector LaunchVelocity = CalculateLaunchVelocity();
		PreviewProjectile->SetActorLocation(LaunchPoint->GetComponentLocation());
		PreviewProjectile->ProjectileMovement->SetActive(true);
		PreviewProjectile->Launch(LaunchVelocity);
        
		PreviewProjectile = nullptr;
        
		FTimerHandle SpawnTimerHandle;
		GetWorldTimerManager().SetTimer(
			SpawnTimerHandle, 
			this, 
			&ASlingShotPawn::OnSpawnDelayComplete, 
			SpawnDelay, 
			false
		);
	} else {
		PreviewProjectile->SetActorLocation(LaunchPoint->GetComponentLocation());
		bIsInSpawnDelay = false;
	}
}

FVector ASlingShotPawn::CalculateLaunchVelocity() {
	FVector LaunchVector = LaunchPoint->GetComponentLocation() - PullPosition;
    
	float PullDistance = LaunchVector.Size();
	float PullFactor = FMath::Clamp(PullDistance / MaxPullDistance, 0.1f, 1.0f);
    
	return LaunchVector * LaunchMultiplier * PullFactor;
}

void ASlingShotPawn::DrawTrajectory(FColor TrajectoryColor) {
	FlushPersistentDebugLines(GetWorld());
    
	if (bValidPullDirection) {
		FVector StartLocation = LaunchPoint->GetComponentLocation();
		FVector Velocity = CalculateLaunchVelocity();
		FVector Gravity = FVector(0.0f, 0.0f, -980.0f);

		FVector PrevPos = StartLocation;
		float Time = 0.0f;
        
		while (Time <= TrajectoryDuration) {
			FVector Position = StartLocation + (Velocity * Time) + (0.5f * Gravity * FMath::Square(Time));
            
			DrawDebugLine(GetWorld(), PrevPos, Position, TrajectoryColor, false, 0.0f, 0, 1.0f);
			DrawDebugPoint(GetWorld(), Position, TrajectoryPointSize, TrajectoryColor, false, 0.0f);
            
			PrevPos = Position;
			Time += TrajectoryTimeStep;
		}
	}
    
	DrawDebugLine(GetWorld(), LaunchPoint->GetComponentLocation(), PullPosition, 
				 bValidPullDirection ? FColor::Green : FColor::Red, false, -1.0f, 0, 2.0f);
}

void ASlingShotPawn::OnSpawnDelayComplete() {
	SpawnPreviewProjectile();
	bIsInSpawnDelay = false;
}