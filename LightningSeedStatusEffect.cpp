#include "LightningSeedStatusEffect.h"
#include "LightningSeedVFXComponent.h"
#include "StatusEffects/ACFForDurationStatusEffect.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "ARSStatisticsComponent.h"
#include "Kismet\KismetSystemLibrary.h"
#include "Interfaces/ACFEntityInterface.h"
#include "Samsara/DamageTypes/AirDamageType.h"

void ULightningSeedStatusEffect::OnStatusEffectStarts_Implementation(ACharacter* Character)
{
	
	Super::OnStatusEffectStarts_Implementation(Character);
	statComp = Character->GetComponentByClass<UARSStatisticsComponent>();
	if (statComp)
	{
		statComp->OnStatisticChanged.AddDynamic(this, &ULightningSeedStatusEffect::OnPoiseLevelChange);
		statComp->OnStatisiticReachesZero.AddDynamic(this, &ULightningSeedStatusEffect::OnHealthReachZero);
	}

	//Add VFX component and spawn particle system
	Character->AddComponentByClass(VFXComponentClassTemplate, false, FTransform::Identity, true);
	VFXComponentRef = Character->GetComponentByClass<ULightningSeedVFXComponent>();
	Character->FinishAddComponent(VFXComponentRef, false, FTransform::Identity);
	Character->FinishAndRegisterComponent(VFXComponentRef);
	VFXComponentRef = Character->GetComponentByClass<ULightningSeedVFXComponent>();
	if (VFXComponentRef) VFXComponentRef->SpawnParticleSystem(Character, GetCenterOfCapsuleComponent());

}

void ULightningSeedStatusEffect::OnStatusEffectEnds_Implementation()
{
	
	if (bTurnOffByStatusEffectManager)
	{
		Super::OnStatusEffectEnds_Implementation();
		//Unbind the events
		if (statComp)
		{
			statComp->OnStatisticChanged.RemoveDynamic(this,&ULightningSeedStatusEffect::OnPoiseLevelChange);
			statComp->OnStatisiticReachesZero.RemoveDynamic(this, &ULightningSeedStatusEffect::OnHealthReachZero);
		}

		//Remove the VFX component and stop the particle system
		if (VFXComponentRef)
		{
			VFXComponentRef->DestroyLightningParticleSystem();
			VFXComponentRef->DestroyComponent();
		}
		
	}
	else
	{
		Super::OnStatusRetriggered_Implementation();
	}
}

void ULightningSeedStatusEffect::TryCastLightningSeedToOthers()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_StatusEffecctLightningSeed_CastToOthers);
	//spawn burst particles with VFX component
	if (VFXComponentRef) VFXComponentRef->SpawnBurstParticleSystem();
	//Run Sphere check to find potential targets
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> ignoreActors;
	ignoreActors.Init(CharacterOwner, 1);
	outActors.Empty();
	if (UKismetSystemLibrary::SphereOverlapActors(CharacterOwner->GetWorld(), GetCenterOfCapsuleComponent(), range, traceObjectTypes, NULL, ignoreActors, outActors))
	{
		for (int i = 0; i < outActors.Num(); i++)
		{
			bool bImplements = outActors[i]->GetClass()->ImplementsInterface(UACFEntityInterface::StaticClass());
			if (bImplements && IACFEntityInterface::Execute_GetEntityCombatTeam(outActors[i]) != eActorTeamToIgnore)
			{
				UARSStatisticsComponent* actorStatComp = outActors[i]->GetComponentByClass<UARSStatisticsComponent>();
				UACFStatusEffectManagerComponent* actorStatusComp = outActors[i]->GetComponentByClass<UACFStatusEffectManagerComponent>();
				//Apply damage
				UGameplayStatics::ApplyDamage(outActors[i], damage, NULL, NULL, UAirDamageType::StaticClass());
				if (actorStatComp) actorStatComp->ModifyStatistic(FGameplayTag::RequestGameplayTag("RPG.Statistics.Poise"), -1 * poiseDamage);
				//Apply status effect
				if (actorStatusComp) actorStatusComp->CreateAndApplyStatusEffect(this->GetClass());
				//Add Lightning Beam through VFX component
				if (VFXComponentRef)
				{
					tempActor = outActors[i];//Debug usage only
					VFXComponentRef->SpawnLightningBeamParticleSystem(tempActor);
				}
			}
		}
	}

	//Stop status effect
	bTurnOffByStatusEffectManager = true;
	if (StatusComp) StatusComp->RemoveStatusEffect(GetStatusEffectTag());

}

FVector ULightningSeedStatusEffect::GetCenterOfCapsuleComponent()
{
	float halfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector worldLocation = CharacterOwner->GetCapsuleComponent()->GetComponentLocation();
	return FVector(worldLocation.X, worldLocation.Y, worldLocation.Z + halfHeight);
}

void ULightningSeedStatusEffect::OnPoiseLevelChange(const FGameplayTag& Stat, float OldValue, float NewValue)
{
	if (Stat == FGameplayTag::RequestGameplayTag("RPG.Statistics.PoiseLevel") && OldValue > NewValue) TryCastLightningSeedToOthers();
}

void ULightningSeedStatusEffect::OnHealthReachZero(FGameplayTag Stat)
{
	if (Stat == FGameplayTag::RequestGameplayTag("RPG.Statistics.Health")) TryCastLightningSeedToOthers();
}
