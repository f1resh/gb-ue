// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Engine/TargetPoint.h"
#include "TankPawn.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class ATankPlayerController;
// class ATankAIController;
class ACannon;

UCLASS()
class TANKOGEDDON_API ATankPawn : public ABasePawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent * SpringArm;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UCameraComponent * Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
	float MoveSpeed = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
	float StrafeSpeed = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
	float RotationSpeed = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
	float InterpolationKey = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Speed")
    float TurretRotationInterpolationKey = 0.5f;

	float TargetForwardAxisValue;
	float TargetMoveRightAxisValue;
	float TargetRightAxisValue;
	float CurrentRightAxisValue;

	UPROPERTY()
	ATankPlayerController* TankController;
	// UPROPERTY()
	// ATankAIController* TankAIController;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon1")
		TSubclassOf<ACannon> CannonClass1;
	ACannon* Cannon1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon2")
		TSubclassOf<ACannon> CannonClass2;
	ACannon* Cannon2;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Patrol points" , Meta = (MakeEditWidget = true))
	TArray<ATargetPoint*> PatrollingPoints;
	// int32 _currentPatrolPointIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Accurency")
    float MovementAccurency = 50;

	int Points = 0;

public:
	// Sets default values for this pawn's properties
	ATankPawn();

	UFUNCTION()
	void MoveForward(float AxisValue);
	UFUNCTION()
	void MoveRight(float AxisValue);
	UFUNCTION()
	void RotateRight(float AxisValue);

	void MoveTank(float DeltaTime);

	UFUNCTION()
	void SwitchCannon();

	UFUNCTION()
	TArray<FVector> GetPatrollingPoints();
	UFUNCTION()
	float GetMovementAccurency() {return MovementAccurency; };
	UFUNCTION()
	FVector GetTurretForwardVector() ;

	UFUNCTION()
	void RotateTurretTo(FVector TargetPosition);

	FVector GetEyesPosition();

	void SetPatrollingPoints(TArray<ATargetPoint*> NewPatrollingPoints);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitCannons();
	void SetupCannon();

	void AddScore(int);

	void Die() override;

	void DamageTaked(float DamageValue) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetupCannon(TSubclassOf<ACannon> NewCannonClass);
	void AddRoundToCurrentCannon(int);

};
