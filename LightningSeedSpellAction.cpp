#include "LightningSeedSpeelAction.h"
#include "LightningSeedStatusEffect.h"
#include "ATSTargetingComponent.h"
#include "Interfaces/ACFEntityInterface.h"
#include "Samsara/Components/SoftlockComponent.h"
#include "GameFramework/GameState.h"
#include <Kismet/GameplayStatics.h>
#include "Samsara/Interfaces/SamsaraGameStateInterface.h"

void ULightningSeed::OnActionStarted_Implementation(const FString& contextString)
{
	Super::OnActionStarted_Implementation(contextString);
	if (GetSpellTarget()->IsValidLowLevel())
	{
		PlaySoundAtLocation(soundToPlayOnCast);
		const bool bImplements = CharacterOwner->GetClass()->ImplementsInterface(UACFEntityInterface::StaticClass());
		if (bImplements)
		{
			ETeam combatTeam = IACFEntityInterface::Execute_GetEntityCombatTeam(CharacterOwner);
			UACFStatusEffectManagerComponent* statusManagerComp = GetSpellTarget()->GetComponentByClass<UACFStatusEffectManagerComponent>();
			if (combatTeam == ETeam::ETeam1)
			{
				//load status effect class with soft pointer
				playerCastLightningSeedStatusClass = playerCastLightningSeedStatusClass.LoadSynchronous();
				if (statusManagerComp->IsValidLowLevel()) statusManagerComp->CreateAndApplyStatusEffect(playerCastLightningSeedStatusClass.Get());
			}
			else
			{
				//load status effect class (enemy casted) with soft pointer
				enemyCastLightningSeedStatusClass = enemyCastLightningSeedStatusClass.LoadSynchronous();
				if (statusManagerComp->IsValidLowLevel()) statusManagerComp->CreateAndApplyStatusEffect(enemyCastLightningSeedStatusClass.Get());
			}
		}
	
	}
}

AActor* ULightningSeed::GetSpellTarget()
{
	UATSTargetingComponent* ATSTargetingComponentPtr = CharacterOwner->GetController()->GetComponentByClass<UATSTargetingComponent>();
	if (CharacterOwner->IsPlayerControlled() && !ATSTargetingComponentPtr->HasTarget())
	{
		AGameStateBase* gameState = UGameplayStatics::GetGameState(CharacterOwner);
		const bool bImplements = gameState->GetClass()->ImplementsInterface(USamsaraGameStateInterface::StaticClass());
		if (bImplements)
		{
			TArray<APawn*> potentialTargets = ISamsaraGameStateInterface::Execute_GetEnemiesInPlayerProximity(gameState);
			return CharacterOwner->GetComponentByClass<USoftlockComponent>()->TriggerSoftLock(potentialTargets,FVector(0,0,0),1000.0f);
		}
	}
	else
	{
		return ATSTargetingComponentPtr->GetCurrentTarget();
	}
	return nullptr;
}

void ULightningSeed::PlaySoundAtLocation(TSoftObjectPtr<USoundCue> soundToPlay)
{
	if (soundToPlay.IsPending())
	{
		soundToPlay = soundToPlay.LoadSynchronous();

	}
	if (soundToPlay.IsValid())
	{
		TSoftObjectPtr<USoundCue> soundCueAsset = soundToPlay.Get();

		if (soundCueAsset)
		{
			UGameplayStatics::PlaySoundAtLocation(this, soundCueAsset.Get(), CharacterOwner->GetActorLocation());

		}
		else
		{
			if (soundCueAsset)
			{
				UGameplayStatics::PlaySoundAtLocation(this, soundCueAsset.Get(), CharacterOwner->GetActorLocation());
			}
		}

	}
}