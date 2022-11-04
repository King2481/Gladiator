// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlastJumpingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UBlastJumpingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlastJumpingComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the component ends play.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		
	// Called when the owner takes damage.
	UFUNCTION()
	void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
