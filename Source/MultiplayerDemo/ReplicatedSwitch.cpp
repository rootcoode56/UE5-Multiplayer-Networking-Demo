// Fill out your copyright notice in the Description page of Project Settings.

#include "ReplicatedSwitch.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AReplicatedSwitch::AReplicatedSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("SceneRoot")
	);

	SetRootComponent(SceneRoot);

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("SwitchMesh")
	);

	SwitchMesh->SetupAttachment(SceneRoot);
}

void AReplicatedSwitch::BeginPlay()
{
	Super::BeginPlay();

	UpdateSwitchVisual();
}

void AReplicatedSwitch::ToggleSwitch()
{
	if (!HasAuthority())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ToggleSwitch rejected: caller does not have authority")
		);

		return;
	}

	bIsActive = !bIsActive;

	UpdateSwitchVisual();

	ForceNetUpdate();
}

void AReplicatedSwitch::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(
		AReplicatedSwitch,
		bIsActive
	);
}

void AReplicatedSwitch::OnRep_IsActive()
{
	UpdateSwitchVisual();
}

void AReplicatedSwitch::UpdateSwitchVisual()
{
	if (!SwitchMesh)
	{
		return;
	}

	const FRotator TargetRotation =
		bIsActive
		? FRotator(0.0f, 0.0f, 35.0f)
		: FRotator::ZeroRotator;

	SwitchMesh->SetRelativeRotation(TargetRotation);
}

