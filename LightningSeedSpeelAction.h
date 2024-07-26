#pragma once

#include "CoreMinimal.h"
#include "../SamsaraBaseSpell_cpp.h"
#include "LightningSeedStatusEffect.h"
#include "LightningSeed.generated.h"

/**
 * 
 */
UCLASS()
class SAMSARA_API ULightningSeed : public USamsaraBaseSpell_cpp
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TSoftObjectPtr<USoundCue> soundToPlayOnCast;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Config")
	TSoftClassPtr<ULightningSeedStatusEffect> playerCastLightningSeedStatusClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Config")
	TSoftClassPtr<ULightningSeedStatusEffect> enemyCastLightningSeedStatusClass;
protected:
	virtual void OnActionStarted_Implementation(const FString& contextString = "") override;
public:
	AActor* GetSpellTarget();
private:
	void PlaySoundAtLocation(TSoftObjectPtr<USoundCue> soundToPlay);
};