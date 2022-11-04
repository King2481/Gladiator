// Created by Bruce Crum


#include "Gladiator/GameFramework/Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);

	SetIsReplicated(true);

	HealthValue = 0.0f;
	MaxHealthValue = 150.0f;
	StartingHealthValue = 150.0f;
	HealthDecrementRate = 5.0f;
	TickInterval = 2.5f;
	SelfDamageMultiplier = 0.25f;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickInterval(TickInterval);

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnOwnerTakeDamage);

	if (GetOwnerRole() >= ROLE_Authority)
	{
		SetHealthValue(StartingHealthValue);
	}
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// The owner should still be valid, but just in case...
	if (GetOwner()) 
	{
		GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &UHealthComponent::OnOwnerTakeDamage);
	}

	Super::EndPlay(EndPlayReason);
}

void UHealthComponent::ModifyHealth(const float Value)
{
	// We call the setter so that way we don't have to put CheckIfShouldTick() in multiple functions
	SetHealthValue(HealthValue + Value);
}

void UHealthComponent::ModifyHealthClamped(const float Value)
{
	const float ClampedValue = FMath::Min<float>(HealthValue + Value, MaxHealthValue);
	SetHealthValue(ClampedValue);
}

void UHealthComponent::SetHealthValue(const float NewValue)
{
	HealthValue = NewValue;
	
	OnHealthUpdatedDelegate.Broadcast(HealthValue);

	CheckShouldTick();
}

float UHealthComponent::GetHealth() const
{
	return HealthValue;
}

float UHealthComponent::GetHealthRatio() const
{
	const float Divider = MaxHealthValue > FLT_EPSILON ? MaxHealthValue : FLT_EPSILON; // Just to be safe.
	return HealthValue / Divider;
}

bool UHealthComponent::IsBeyondMaxHealth() const
{
	return HealthValue > MaxHealthValue;
}

void UHealthComponent::CheckShouldTick()
{
	if (HealthValue > MaxHealthValue && !IsComponentTickEnabled())
	{
		SetComponentTickEnabled(true);
	}
	else if (HealthValue <= MaxHealthValue && IsComponentTickEnabled())
	{
		SetComponentTickEnabled(false);
	}
}

void UHealthComponent::OnHealthChanged()
{
	OnHealthUpdatedDelegate.Broadcast(HealthValue);
	
	CheckShouldTick();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UHealthComponent, HealthValue);
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float Highest = FMath::Max<float>(HealthValue - (HealthDecrementRate * DeltaTime), MaxHealthValue);
	SetHealthValue(Highest);
}

void UHealthComponent::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	float ActualDamage = Damage;

	if (GetOwner()->GetInstigatorController() == InstigatedBy)
	{
		ActualDamage *= SelfDamageMultiplier;
	}

	ModifyHealth(-ActualDamage);

	if (HealthValue <= 0.0f)
	{
		OnHealthDepletedDelegate.Broadcast(Damage, DamageType, InstigatedBy, DamageCauser);
	}
}