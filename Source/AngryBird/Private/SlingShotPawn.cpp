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

	// Adjusted SpringArm for better framing
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f; // Pull back slightly for wider view
	SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f)); // Angle down more
	SpringArm->SetRelativeLocation(FVector(-100.0f, 0.0f, 150.0f)); // Higher and closer
	SpringArm->bEnableCameraLag = true; // Smooth movement
	SpringArm->CameraLagSpeed = 5.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->FieldOfView = 90.0f; // Wider FOV for better context

	LaunchPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LaunchPoint"));
	LaunchPoint->SetupAttachment(RootComponent);
	LaunchPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

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

			float T = (LaunchPoint->GetComponentLocation().Z - WorldPos.Z) / WorldDir.Z;
			PullPosition = WorldPos + WorldDir * T;

			DrawTrajectory();
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

void ASlingShotPawn::PullSlingshot(const FInputActionValue& Value) {
	bIsPulling = Value.Get<bool>();

	if (bIsPulling && !PreviewProjectile && ProjectileClass) {
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

void ASlingShotPawn::ReleaseSlingshot(const FInputActionValue& Value) {
	if (!bIsPulling || !ProjectileClass || !PreviewProjectile) {
		return;
	}

	bIsPulling = false;

	FVector LaunchVelocity = CalculateLaunchVelocity();
	PreviewProjectile->SetActorLocation(LaunchPoint->GetComponentLocation());
	PreviewProjectile->ProjectileMovement->SetActive(true);
	PreviewProjectile->Launch(LaunchVelocity);
	PreviewProjectile = nullptr;
}

FVector ASlingShotPawn::CalculateLaunchVelocity() {
	FVector LaunchVector = LaunchPoint->GetComponentLocation() - PullPosition;
	return LaunchVector * LaunchMultiplier;
}

void ASlingShotPawn::DrawTrajectory() {
	FVector StartLocation = LaunchPoint->GetComponentLocation();
	FVector Velocity = CalculateLaunchVelocity();
	FVector Gravity = FVector(0.0f, 0.0f, -980.0f);

	float Time = 0.0f;
	while (Time <= TrajectoryDuration) {
		FVector Position = StartLocation + (Velocity * Time) + (0.5f * Gravity * FMath::Square(Time));
		DrawDebugPoint(GetWorld(), Position, TrajectoryPointSize, FColor::Yellow, false, 0.1f);
		Time += TrajectoryTimeStep;
	}
}

void ASlingShotPawn::UpdatePreviewProjectile() {
	if (!PreviewProjectile) return;

	PreviewProjectile->SetActorLocation(PullPosition);

	// Tilt based on lateral offset
	FVector Forward = SpringArm->GetForwardVector();
	FVector ToPull = (PullPosition - LaunchPoint->GetComponentLocation()).GetSafeNormal();
	FVector Right = Forward.Cross(FVector::UpVector);
	float LateralOffset = FVector::DotProduct(ToPull, Right);
	float TiltAngle = FMath::Clamp(LateralOffset * 90.0f, -45.0f, 45.0f);
	FRotator TiltRotation = FRotator(0.0f, 0.0f, TiltAngle);
	PreviewProjectile->SetActorRotation(TiltRotation);
}