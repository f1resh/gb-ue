// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTaker.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "Cannon.h"
#include "BasePawn.generated.h"

UCLASS()
class TANKOGEDDON_API ABasePawn : public APawn, public IDamageTaker
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* BodyMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TurretMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonSetupPoint;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;

	TSubclassOf<ACannon>* CannonClassPtr;
	ACannon** Cannon;

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	UFUNCTION()
		void Fire();
	UFUNCTION()
		void FireSpecial();


	UFUNCTION()
		void TakeDamage(FDamageData DamageData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void Die();

	UFUNCTION()
		virtual void DamageTaked(float DamageValue);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
