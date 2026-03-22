// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/Scene.h"   
#include "NiagaraDataInterfaceExport.h" 
#include "C_FX_PostBlurEffect.generated.h"


class ANiagaraActor;
class UPostProcessComponent;


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SONGGUFU_VFX_V2_API UC_FX_PostBlurEffect : public USceneComponent, public INiagaraParticleCallbackHandler
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_FX_PostBlurEffect();

	// 声明（BlueprintNativeEvent）
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ReceiveParticleData(
		const TArray<FBasicParticleData>& Data,
		UNiagaraSystem* NiagaraSystem,
		const FVector& SimulationPositionOffset);

	// C++实现
	virtual void ReceiveParticleData_Implementation(
		const TArray<FBasicParticleData>& Data,
		UNiagaraSystem* NiagaraSystem,
		const FVector& SimulationPositionOffset) override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
protected:

	UPROPERTY(EditAnywhere)
	UMaterialInterface* InMaterial;

	

private:
	//动态材质
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	//储存场景类
	TArray<AActor*> ScreenComp;

	ANiagaraActor* OwnerNiagara;

	UPostProcessComponent* PostComp;

	FPostProcessSettings* TempSettings;

	//后期材质数组
	TArray<FWeightedBlendable>* TempBlendables;

	//后期材质数组Index
	int32 Index;
};
