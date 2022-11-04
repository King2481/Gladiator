// Created by Bruce Crum


#include "Gladiator/GameFramework/Components/BlastJumpingComponent.h"
#include "Gladiator/GameFramework/GladiatorDamageType.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UBlastJumpingComponent::UBlastJumpingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBlastJumpingComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UBlastJumpingComponent::OnOwnerTakeDamage);
}

void UBlastJumpingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// The owner should still be valid, but just in case...
	if (GetOwner())
	{
		GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &UBlastJumpingComponent::OnOwnerTakeDamage);
	}

	Super::EndPlay(EndPlayReason);
}

void UBlastJumpingComponent::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	const auto OwningCharacter = Cast<ACharacter>(GetOwner());
	if (OwningCharacter)
	{
		const auto GladiatorDamageType = Cast<UGladiatorDamageType>(DamageType);
		if (GladiatorDamageType)
		{
			const FVector LaunchVelocity = (GetOwner()->GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal() * GladiatorDamageType->LaunchMagnitude;
			OwningCharacter->LaunchCharacter(LaunchVelocity, false, false);	
		}
	}
}
