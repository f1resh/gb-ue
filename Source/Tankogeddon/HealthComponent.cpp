// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::TakeDamage(FDamageData DamageData)
{
	float takedDamageValue = DamageData.DamageValue; 
	CurrentHealth -= takedDamageValue;

	if(CurrentHealth <= 0)
	{
		if(OnDie.IsBound())
			OnDie.Broadcast();
	}
	else
	{
		if(OnDamaged.IsBound())
			OnDamaged.Broadcast(takedDamageValue);
	}
}

float UHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetHealthState() const
{
	return CurrentHealth / MaxHealth;
}

void UHealthComponent::AddHealth(float AddiditionalHealthValue)
{
	CurrentHealth += AddiditionalHealthValue;
	if(CurrentHealth > MaxHealth)
		CurrentHealth = MaxHealth;
}
