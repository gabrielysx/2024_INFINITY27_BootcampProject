#pragma once

#include "CoreMinimal.h"
#include "LightningSeedVFXComponent.h"
#include "StatusEffects/ACFForDurationStatusEffect.h"
#include "Interfaces/ACFEntityInterface.h"
#include "ARSStatisticsComponent.h"
#include <Kismet/GameplayStatics.h>
#include "LightningSeedStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class SAMSARA_API ULightningSeedStatusEffect : public UACFForDurationStatusEffect
{
	GENERATED_BODY()

//Variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Config")
	ETeam eActorTeamToIgnore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Config")
	float damage = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Config")
	float poiseDamage = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Config")
	float range = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Config")
	TSubclassOf<UActorComponent> VFXComponentClassTemplate;
private:
	bool bTurnOffByStatusEffectManager;
	UARSStatisticsComponent* statComp;
	ULightningSeedVFXComponent* VFXComponentRef;
	AActor* tempActor;
	TArray<AActor*> outActors;

//Functions
protected:
	virtual void OnStatusEffectStarts_Implementation(ACharacter* Character) override;
	virtual void OnStatusEffectEnds_Implementation() override;

private:
	void TryCastLightningSeedToOthers();
	FVector GetCenterOfCapsuleComponent();
	UFUNCTION()
	void OnPoiseLevelChange(const FGameplayTag& Stat, float OldValue, float NewValue);
	UFUNCTION()
	void OnHealthReachZero(FGameplayTag Stat);
};