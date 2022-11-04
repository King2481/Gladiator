// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdatedDelegate, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthDepletedDelegate, float, Damage, const UDamageType*, DamageType, AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UHealthComponent();

	// Modifies the health by the specified value.
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void ModifyHealth(const float Value);

	// Modifies the health by the specified value, but does not go beyond max health.
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void ModifyHealthClamped(const float Value);

	// This will set the health component to this value. 
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void SetHealthValue(const float NewValue);

	// Returns the current health value
	UFUNCTION(BlueprintPure, Category = "Health Component")
	float GetHealth() const;

	// Returns the current health value as a ratio
	UFUNCTION(BlueprintPure, Category = "Health Component")
	float GetHealthRatio() const;

	// Checks to see if we are past MaxHealthValue
	UFUNCTION(BlueprintPure, Category = "Health Component")
	bool IsBeyondMaxHealth() const;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the component ends play.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called to setup replication.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// What is the currently stored health value on this component?
	UPROPERTY(Replicated, ReplicatedUsing=OnHealthChanged, BlueprintReadOnly, Category = "Health Component")
	float HealthValue;

	UFUNCTION()
	void OnHealthChanged();

	// What is the maximum allowed value? If above this, the health component will tick down to this value.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component")
	float MaxHealthValue;

	// When this component is initialized, what is the starting health value?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component")
	float StartingHealthValue;

	// When decrementing the health every frame, what is it's rate?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component")
	float HealthDecrementRate;

	// When the tick portion of the code is activated, what is it's ticks fequency?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component")
	float TickInterval;

	// If we take damage from ourselves, what is the damage multiplier?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component")
	float SelfDamageMultiplier;

	// Checks to see if we should set tick enabled on this component.
	void CheckShouldTick();

	// Called when the owning actors takes damage
	UFUNCTION()
	void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called when the health actually updates.
	UPROPERTY(BlueprintAssignable)
	FOnHealthUpdatedDelegate OnHealthUpdatedDelegate;

	// Called when the health has reached 0
	UPROPERTY(BlueprintAssignable)
	FOnHealthDepletedDelegate OnHealthDepletedDelegate;
		
};
