// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoComponent.generated.h"

class UAmmoType;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoUpdatedDelegate, int32, NewAmmo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UAmmoComponent();

	// Variable replication setup
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// What type of ammo does this component use?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ammo Component")
	UAmmoType* AmmoType;

	// Sets the ammo to a specified amount (setter)
	UFUNCTION(BlueprintCallable, Category = "Ammo Component")
	void SetAmmo(int32 Count);

	// Modifies the ammo by a specified amount (adder)
	UFUNCTION(BlueprintCallable, Category = "Ammo Component")
	void ModifyAmmo(int32 Count);

	// Returns the ammount of ammo we have
	UFUNCTION(BlueprintCallable, Category = "Ammo Component")
	int32 GetAmmo() const;

	// Do we actually have any rounds to fire?
	UFUNCTION(BlueprintCallable, Category = "Ammo Component")
	bool HasAmmo() const;

protected:

	// How much ammo is currently stored in this component?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Ammo Component")
	int32 Ammo;
	
	UFUNCTION()
	void OnRep_Ammo();

public:

	// Called when the ammo updates updates.
	UPROPERTY(BlueprintAssignable)
	FOnAmmoUpdatedDelegate OnAmmoUpdatedDelegate;
};
