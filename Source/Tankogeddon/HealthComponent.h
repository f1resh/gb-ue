// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStructs.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKOGEDDON_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
    DECLARE_EVENT(UHealthComponent, FOnDie)
    DECLARE_EVENT_OneParam(UHealthComponent, FOnHealthChanged, float)

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health values")
    float MaxHealth = 10;

	UPROPERTY()
    float CurrentHealth;

public:
    FOnDie OnDie;
	FOnHealthChanged OnDamaged;
	
public:	
        // Sets default values for this component's properties
    UHealthComponent();

	void TakeDamage(FDamageData DamageData);

	float GetHealth() const;

	float GetHealthState() const;

	void AddHealth(float AddiditionalHealthValue);
};

