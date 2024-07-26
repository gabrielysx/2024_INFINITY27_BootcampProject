#include "LightningSeedVFXComponent.h"

#include "UObject/Object.h"
#include "Kismet/KismetMathLibrary.h"
#include <NiagaraFunctionLibrary.h>
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "ATSTargetPointComponent.h"

// Sets default values for this component's properties
ULightningSeedVFXComponent::ULightningSeedVFXComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	//PrimaryComponentTick.RegisterTickFunction(GetComponentLevel());
	// ...
}


// Called when the game starts
void ULightningSeedVFXComponent::BeginPlay()
{
	Super::BeginPlay();

	currentChargeProgress = 0.f;
	beamTargetRefs = { nullptr };
	beamParticleSystems = { nullptr };
}

void ULightningSeedVFXComponent::BeginDestroy()
{
	Super::BeginDestroy();

	DestroyLightningParticleSystem();
}


// Called every frame
void ULightningSeedVFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Update current charging progress
	float c_change = chargeProgressChangeRate * DeltaTime;
	if (currentChargeProgress < targetChargeProgress - chargeProgressChangeRate / 100) currentChargeProgress += c_change;
	else if (currentChargeProgress > targetChargeProgress + chargeProgressChangeRate / 100) currentChargeProgress -= c_change;
	else currentChargeProgress = targetChargeProgress;

	//Update Beams positions
	for (int i = 0; i < beamTargetRefs.Num(); i++)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("Location: %d"), i));
		if (beamParticleSystems[i] && beamTargetRefs[i] && !beamParticleSystems[i]->IsBeingDestroyed() && beamTargetRefs[i]->IsValidLowLevel() && beamParticleSystems[i]->IsValidLowLevel())
		{
			FVector beamEndlocation = GetTargetPointPosition(beamTargetRefs[i]);
			SetBeamStartAndEndPosition(beamParticleSystems[i], beamEndlocation);
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Location: %s"), *beamEndlocation.ToString()));
		}
	
		if (!beamTargetRefs[i] || beamParticleSystems[i]->IsBeingDestroyed())
		{
			beamTargetRefs.RemoveAt(i);
			beamParticleSystems.RemoveAt(i);
		}
	}

	//Update particle effects
	lightningVFXParticleSystem->SetNiagaraVariableFloat("User.arcSpawnRate", CalculateValueThroughCurrentChargeProgress(arcMinSpawnRate,arcMaxSpawnRate));
	lightningVFXParticleSystem->SetNiagaraVariableFloat("User.particleSpawnInterval", CalculateValueThroughCurrentChargeProgress(particlesMaxSpawnInterval, particlesMinSpawnInterval));
}

void ULightningSeedVFXComponent::DestroyLightningParticleSystem()
{
	if (lightningVFXParticleSystem)
	{
		lightningVFXParticleSystem->Deactivate();
		lightningVFXParticleSystem->SetAutoDestroy(true);
	}
}

void ULightningSeedVFXComponent::SpawnParticleSystem(AActor* character, FVector spawnLocation)
{
	characterRef = character;
	lightningVFXParticleSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(
		lightningVFXParticleSystemTemplate,
		characterRef->GetComponentByClass<USkeletalMeshComponent>(),
		FName(),
		spawnLocation,
		FRotator(0, 0, 0),
		EAttachLocation::Type::KeepWorldPosition,
		false
	);
}

void ULightningSeedVFXComponent::SpawnLightningBeamParticleSystem_Implementation(AActor* beamTargetActor)
{
	beamTargetRefs.Push(beamTargetActor);
	UNiagaraComponent* beamVFXSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(
		beamParticleSystemTemplate,
		characterRef->GetComponentByClass<USkeletalMeshComponent>(),
		FName(),
		GetTargetPointPosition(characterRef),
		FRotator(0, 0, 0),
		EAttachLocation::Type::KeepWorldPosition,
		true
	);
	beamParticleSystems.Push(beamVFXSystem);
	SetBeamStartAndEndPosition(beamVFXSystem, GetTargetPointPosition(beamTargetActor));
}

void ULightningSeedVFXComponent::SpawnBurstParticleSystem()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(
		burstParticleSystemTemplate,
		characterRef->GetComponentByClass<USkeletalMeshComponent>(),
		FName(),
		GetTargetPointPosition(characterRef),
		FRotator(0, 0, 0),
		EAttachLocation::Type::KeepWorldPosition,
		true
	)->SetNiagaraVariableFloat("User.burstParticleCount", burstParticlesSpawnCount);
}

FVector ULightningSeedVFXComponent::GetTargetPointPosition(AActor* actor)
{
	UATSTargetPointComponent* targetPointComp = actor->GetComponentByClass<UATSTargetPointComponent>();
	if (targetPointComp) return targetPointComp->GetComponentLocation();
	return FVector();
}

void ULightningSeedVFXComponent::SetBeamStartAndEndPosition(UNiagaraComponent* beamNiagaraSystem, FVector endPosition)
{
	FVector startPosition;
	if (characterRef) startPosition = GetTargetPointPosition(characterRef);
	beamNiagaraSystem->SetNiagaraVariableVec3("User.BeamStartPoint", startPosition);
	beamNiagaraSystem->SetNiagaraVariableVec3("User.BeamEndPoint", endPosition);
}

float ULightningSeedVFXComponent::CalculateValueThroughCurrentChargeProgress(float min, float max)
{
	return UKismetMathLibrary::MapRangeClamped(currentChargeProgress, 0.0, 1.0, min, max);
}