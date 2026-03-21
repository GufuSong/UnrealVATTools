// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraDataInterfaceExport.h"
#include "C_VAT.generated.h"


class UMaterialInstanceConstant;
class UStaticMeshComponent;
class INiagaraParticleCallbackHandler;


USTRUCT(BlueprintType)
struct FStMaterialSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceConstant* Material;



	FStMaterialSlot()
		: Index(0)
		, Material(nullptr)
	{ }

	FStMaterialSlot(int32 InIndex, UMaterialInstanceConstant* InMat)
		: Index(InIndex)
		, Material(InMat)
	{ }
};




UCLASS()
class SONGGUFU_VFX_V2_API AC_VAT : public AActor, public INiagaraParticleCallbackHandler
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AC_VAT();

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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStMaterialSlot> AllMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* InVATMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OffsetPosition = FVector(0, 0, -100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector(1, 1, 1);


private:
	//动态材质
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	FVector InitialPosition;

	UStaticMeshComponent* MeshComponent;
};