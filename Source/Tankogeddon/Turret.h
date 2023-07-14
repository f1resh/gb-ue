// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePawn.h"
#include "Scorable.h"

#include "Turret.generated.h"

UCLASS()
class TANKOGEDDON_API ATurret : public ABasePawn, public IScorable
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
    TSubclassOf<ACannon> CannonClass;

	UPROPERTY()
    ACannon* CannonPtr;
	UPROPERTY()
    APawn * PlayerPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
    float TargetingRange = 1000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
    float TargetingSpeed = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
    float TargetingRate = 0.005f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
    float Accurency = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
	float SwitchFireModeTimer = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
	bool BallisticTargeting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score points")
	int Points = 5;

	const FString BodyMeshPath = "StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'";
	const FString TurretMeshPath = "StaticMesh'/Game/CSC/Meshes/SM_CSC_Gun1.SM_CSC_Gun1'";
public:	
    ATurret();

	UFUNCTION()
	int GivePoints();

	virtual void PostInitializeComponents() override;
protected:
    virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void Targeting();

	void RotateToPlayer();

	void ElevateCannon();

	bool IsPlayerInRange();

	bool IsPlayerSeen();

	bool CanFire();

    void Die() override;
 
    void DamageTaked(float DamageValue) override;

	void SwitchFireMode();

	FVector GetEyesPosition();
	

};
