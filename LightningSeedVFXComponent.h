#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <NiagaraFunctionLibrary.h>
#include "LightningSeedVFXComponent.generated.h"


UCLASS(Blueprintable)
class SAMSARA_API ULightningSeedVFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULightningSeedVFXComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

//Variables
public:
	float targetChargeProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Variables", meta = (ToolTip = "This decides how quick will the charging effect transite (1 means transitioning from 0 to 1 in 1 sec )"))
	float chargeProgressChangeRate = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Variables")
	float arcMaxSpawnRate = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Variables")
	float arcMinSpawnRate = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Variables")
	float particlesMaxSpawnInterval = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Variables")
	float particlesMinSpawnInterval = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Variables")
	int32 burstParticlesSpawnCount = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Particle System Templates")
	UNiagaraSystem* lightningVFXParticleSystemTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Particle System Templates")
	UNiagaraSystem* beamParticleSystemTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX Assets|Particle System Templates")
	UNiagaraSystem* burstParticleSystemTemplate;
	UPROPERTY(BlueprintReadOnly)
	AActor* characterRef;
	TArray<AActor*> beamTargetRefs;
private:
	float currentChargeProgress;
	UNiagaraComponent* lightningVFXParticleSystem;
	TArray<UNiagaraComponent*> beamParticleSystems;

//Functions
public:
	void DestroyLightningParticleSystem();
	void SpawnParticleSystem(AActor* characterRef, FVector spawnLocation);
	UFUNCTION(BlueprintNativeEvent)
	void SpawnLightningBeamParticleSystem(AActor* beamTargetActor);
	virtual void SpawnLightningBeamParticleSystem_Implementation(AActor* beamTargetActor);
	void SpawnBurstParticleSystem();
private:
	UFUNCTION(BlueprintCallable, Category = "My Functions")
	FVector GetTargetPointPosition(AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "My Functions")
	void SetBeamStartAndEndPosition(UNiagaraComponent* beamNiagaraSystem, FVector endPosition);
	float CalculateValueThroughCurrentChargeProgress(float min, float max);
};