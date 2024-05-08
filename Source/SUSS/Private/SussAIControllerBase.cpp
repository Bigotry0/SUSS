﻿
#include "SussAIControllerBase.h"

#include "SussBrainComponent.h"

ASussAIControllerBase::ASussAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	BrainComponent = CreateDefaultSubobject<USussBrainComponent>("SUSS Brain");
	AddInstanceComponent(BrainComponent);

	// We need StartLogic to be called for our brain to work, doing it on possess is easiest
	// Not sure why this isn't default, perhaps because you're supposed to activate it when players come near
	// However our brain component already accounts for distance & throttles when far away
	bStartAILogicOnPossess = true;
	
}

USussBrainComponent* ASussAIControllerBase::GetSussBrainComponent() const
{
	return Cast<USussBrainComponent>(BrainComponent);
}

const FSussBrainConfig& ASussAIControllerBase::GetBrainConfig() const
{
	if (auto B = GetSussBrainComponent())
	{
		return B->GetBrainConfig();
	}

	static FSussBrainConfig Dummy;
	return Dummy;
}

void ASussAIControllerBase::SetBrainConfig(const FSussBrainConfig& NewConfig)
{
	if (auto B = GetSussBrainComponent())
	{
		B->SetBrainConfig(NewConfig);
	}
}

void ASussAIControllerBase::SetBrainConfigFromAsset(USussBrainConfigAsset* Asset)
{
	if (auto B = GetSussBrainComponent())
	{
		B->SetBrainConfigFromAsset(Asset);
	}
}

void ASussAIControllerBase::StopCurrentAction()
{
	if (auto B = GetSussBrainComponent())
	{
		B->StopCurrentAction();
	}
}

FVector ASussAIControllerBase::GetFocalPointOnActor(const AActor* Actor) const
{
	if (!Actor)
		return FAISystem::InvalidLocation;

	const FVector FocusBaseLoc = Actor->GetActorLocation();
	const APawn* AgentPawn = GetPawn();
	if (AgentPawn && LeadTargetProjectileVelocity > 0)
	{
		// Use simple first-order correction for deflection
		// This is not 100% accurate (doesn't even account for collision boxes) but is close enough; not being
		// perfect is a nice AI feature after all

		// We simply calculate the time it would take the projectile to hit the target where it is now, and calculate
		// where it will be in that time and aim at that instead
		const float TimeToHitCurrent = FVector::Distance(AgentPawn->GetActorLocation(), FocusBaseLoc) / LeadTargetProjectileVelocity;
		return FocusBaseLoc + TimeToHitCurrent * Actor->GetVelocity();
	}
	else
	{
		return FocusBaseLoc;
	}
}
