// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "TankPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
//#include "TankAIController.h"

DECLARE_LOG_CATEGORY_EXTERN(TankLog, All, All);
DEFINE_LOG_CATEGORY(TankLog);

// Sets default values
ATankPawn::ATankPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank turret"));
	TurretMesh->SetupAttachment(BodyMesh);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(BodyMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

}

FVector ATankPawn::GetTurretForwardVector()
{
	return TurretMesh->GetForwardVector();	
}



// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<ATankPlayerController>(GetController());
	//TankAIController = Cast<ATankAIController>(GetController());
	InitCannons();
	
	if(!GetController())
	{
		SpawnDefaultController();
	}
}

void ATankPawn::InitCannons()
{
	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	Cannon1 = GetWorld()->SpawnActor<ACannon>(CannonClass1, params);
	Cannon1->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Cannon1->OnGetScore.AddUObject(this, &ATankPawn::AddScore);
	Cannon2 = GetWorld()->SpawnActor<ACannon>(CannonClass2, params);
	Cannon2->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Cannon2->OnGetScore.AddUObject(this, &ATankPawn::AddScore);

	Cannon = &Cannon1;
	CannonClassPtr = &CannonClass1;
}

void ATankPawn::SetupCannon()
{
	if (Cannon)
	{
		(*Cannon)->Destroy();
	}

	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	(*Cannon) = GetWorld()->SpawnActor<ACannon>(*CannonClassPtr, params);
	(*Cannon)->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	(*Cannon)->OnGetScore.AddUObject(this, &ATankPawn::AddScore);
}

void ATankPawn::SetupCannon(TSubclassOf<ACannon> NewCannonClass)
{
	*CannonClassPtr = NewCannonClass;
	SetupCannon();
}

void ATankPawn::MoveForward(float AxisValue)
{
	TargetForwardAxisValue = AxisValue;
}

void ATankPawn::MoveRight(float AxisValue)
{
	TargetMoveRightAxisValue = AxisValue;
}

void ATankPawn::RotateRight(float AxisValue)
{
	TargetRightAxisValue = AxisValue;
}

void ATankPawn::MoveTank(float DeltaTime)
{
	FVector currentLocation = GetActorLocation();
	FVector forwardVector = GetActorForwardVector();
	FVector rightVector = GetActorRightVector();
	FVector movePosition = currentLocation + (forwardVector * MoveSpeed * TargetForwardAxisValue + rightVector * StrafeSpeed * TargetMoveRightAxisValue) * DeltaTime;
	SetActorLocation(movePosition, true);
}

void ATankPawn::SwitchCannon()
{
	if (Cannon == &Cannon1) {
		Cannon = &Cannon2;
		CannonClassPtr = &CannonClass2;
	}
	else if (Cannon == &Cannon2){
		Cannon = &Cannon1;
		CannonClassPtr = &CannonClass1;
	}
	FString name = (*Cannon)->GetClass()->GetName();
	UE_LOG(TankLog, Display, TEXT("Switch to cannon: %s"), *name);
}


// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tank movement
	if (TargetForwardAxisValue != 0 || TargetMoveRightAxisValue!=0) MoveTank(DeltaTime);


	// Tank rotation
	CurrentRightAxisValue = FMath::Lerp(CurrentRightAxisValue, TargetRightAxisValue, InterpolationKey);
	//UE_LOG(TankLog, Warning, TEXT("CurrentRightAxisValue = %f TargetRightAxisValue = %f"), CurrentRightAxisValue, TargetRightAxisValue);
	float yawRotation = RotationSpeed * CurrentRightAxisValue * DeltaTime;
	FRotator currentRotation = GetActorRotation();
	yawRotation = currentRotation.Yaw + yawRotation;
	FRotator newRotation = FRotator(0, yawRotation, 0);
	SetActorRotation(newRotation);


	// Turret rotation
	if(TankController)
	{
		FVector mousePos = TankController->GetMousePos();
		RotateTurretTo(mousePos);
		// FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mousePos);
		// FRotator currRotation = TurretMesh->GetComponentRotation();
		// targetRotation.Pitch = currRotation.Pitch;
		// targetRotation.Roll = currRotation.Roll;
		// TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TurretRotationInterpolationKey));
	}
}

void ATankPawn::RotateTurretTo(FVector TargetPosition)
{
	//FVector mousePos = TankController->GetMousePos();
	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPosition);
	FRotator currRotation = TurretMesh->GetComponentRotation();
	targetRotation.Pitch = currRotation.Pitch;
	targetRotation.Roll = currRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TurretRotationInterpolationKey));
}

FVector ATankPawn::GetEyesPosition()
{
	return CannonSetupPoint->GetComponentLocation();
}

void ATankPawn::SetPatrollingPoints(TArray<ATargetPoint*> NewPatrollingPoints)
{
	FVector scale = GetActorScale3D();
	PatrollingPoints = NewPatrollingPoints;
}

// Called to bind functionality to input
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATankPawn::AddRoundToCurrentCannon(int number)
{
	(*Cannon)->AddRounds(number);
}


TArray<FVector> ATankPawn::GetPatrollingPoints()
{
	TArray<FVector> points;
	for (ATargetPoint* point: PatrollingPoints)
	{
		points.Add(point->GetActorLocation());
	}

	return points;
}

void ATankPawn::Die()
{
	if(Cannon1)
		Cannon1->Destroy();
	if (Cannon2)
		Cannon2->Destroy();
	if (Cannon)
		delete Cannon;
	
	Destroy();
}

void ATankPawn::DamageTaked(float DamageValue)
{
	UE_LOG(TankLog, Warning, TEXT("Tank %s took damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

void ATankPawn::AddScore(int score)
{
	Points += score;
	GEngine->AddOnScreenDebugMessage(10, 5, FColor::Red, FString::Printf(TEXT("Total score: %d"), Points));
}
