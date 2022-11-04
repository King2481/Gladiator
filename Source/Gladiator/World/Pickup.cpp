// Created by Bruce Crum


#include "Gladiator/World/Pickup.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gladiator/Characters/GladiatorCharacter.h"
#include "Gladiator/Player/GladiatorPlayerController.h"

// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CollisionComp = CreateOptionalDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionProfileName("Pickup");

	SetRootComponent(CollisionComp);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->SetupAttachment(RootComponent);
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->CastShadow = false;

	bEnabled = true;
	DisableTime = 15.f;
	PickupSound = nullptr;
	bStartsDisabled = false;
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bEnabled);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlap);

	if (HasAuthority() && bStartsDisabled)
	{
		bEnabled = false;
		HandleVisibility();

		GetWorldTimerManager().SetTimer(DisableTimerHandle, this, &APickup::OnReenabled, DisableTime);
	}
}

void APickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	const auto Character = Cast<AGladiatorCharacter>(OtherActor);
	if (!Character || !CanPickup(Character))
	{
		return;
	}

	HandlePickup(Character);

	bEnabled = false;
	HandleVisibility();

	GetWorldTimerManager().SetTimer(DisableTimerHandle, this, &APickup::OnReenabled, DisableTime);
}

bool APickup::CanPickup(AGladiatorCharacter* ForUser) const
{
	return BlueprintCanPickup(ForUser);
}

void APickup::HandlePickup(AGladiatorCharacter* ForUser)
{
	const auto PC = ForUser->GetController<AGladiatorPlayerController>();
	if (PC && PickupSound)
	{
		PC->ClientPlaySound2D(PickupSound);
	}

	BlueprintHandlePickup(ForUser);
}

void APickup::OnRep_Enabled()
{
	HandleVisibility();
}

void APickup::HandleVisibility()
{
	SetActorHiddenInGame(!bEnabled);
	CollisionComp->SetGenerateOverlapEvents(bEnabled);
}

void APickup::OnReenabled()
{
	bEnabled = true;
	HandleVisibility();
}