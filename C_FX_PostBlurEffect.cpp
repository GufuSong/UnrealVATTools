// Fill out your copyright notice in the Description page of Project Settings.


#include "C_FX_PostBlurEffect.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values for this component's properties
UC_FX_PostBlurEffect::UC_FX_PostBlurEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 初始化InMaterial
	// /Script/Engine.Material'/Game/SongGuFu_VFXContent/NiagataEmitter/Post/NE_Component_PostBlur/Material/MF/M_Post_Blur.M_Post_Blur'
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(
		TEXT("/Game/SongGuFu_VFXContent/NiagataEmitter/Post/NE_Component_PostBlur/Material/MF/M_Post_Blur.M_Post_Blur")
	);

	if (Mat.Succeeded())
	{
		InMaterial = Mat.Object;
	}


}




// Called when the game starts
void UC_FX_PostBlurEffect::BeginPlay()
{
	Super::BeginPlay();

	// 初始化Niagara
	OwnerNiagara = Cast<ANiagaraActor>(GetOwner());

	if (!OwnerNiagara){
		UE_LOG(LogTemp, Error, TEXT("Owner is not NiagaraActor"));
		return;
	}
	OwnerNiagara->GetNiagaraComponent()->SetNiagaraVariableObject(TEXT("User.PostPass_Blur_CallBackObject"), this);

	//获取蓝图类
	///Script/Engine.Blueprint'/Game/SongGuFu_VFXContent/GameBP/Environment/B_Envrionment.B_Envrionment'
	UClass* BPClass = LoadClass<AActor>(
		nullptr,
		TEXT("/Game/SongGuFu_VFXContent/GameBP/Environment/B_Envrionment.B_Envrionment_C")
	);

	if (!BPClass)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadClass failed"));
		return;
	}

	//获取场景组件
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		BPClass,
		ScreenComp
	);

	if (ScreenComp.Num() != 1) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Error: There are multiple components present in the scene"))
		);

		return;
	}

	//初始化材质
	DynamicMaterials.Add(
		UMaterialInstanceDynamic::Create(InMaterial, this)
	);

	//添加Post组件声明
	//获取Post组件
	PostComp = ScreenComp[0]->FindComponentByClass<UPostProcessComponent>();
	
	FPostProcessSettings& Settings = PostComp->Settings;
	TempSettings = &Settings;

	TArray<FWeightedBlendable>& Blendables = Settings.WeightedBlendables.Array;
	TempBlendables = &Blendables;

	Index = TempBlendables->Add(
		FWeightedBlendable{ 1.0f, DynamicMaterials[0]}
	);


}

void UC_FX_PostBlurEffect::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset)
{
	if (Data.Num() == 0) return;
	//Niagara仅发射一个粒子

	//实现生命周期
	if (Index != INDEX_NONE && Data[0].Size < 0.5)
	{
		if (TempBlendables && TempBlendables->IsValidIndex(Index))
		{
			(*TempBlendables)[Index].Weight = 0;	//暂停使用
			TempBlendables->RemoveAt(Index);

			Index = INDEX_NONE;
			return;
		}
	}
	else { return; }
	
	//参数传递
	UMaterialInstanceDynamic* TempDYMat = DynamicMaterials[0];

	TempDYMat->SetScalarParameterValue(TEXT("Radial"), Data[0].Velocity[0]);
	TempDYMat->SetScalarParameterValue(TEXT("BlurStrength"), Data[0].Velocity[1]);
	TempDYMat->SetScalarParameterValue(TEXT("GlobalBlurIntensity"), Data[0].Velocity[2]);

	TempDYMat->SetVectorParameterValue(TEXT("PositionWS"), Data[0].Position);


}




// Called every frame
void UC_FX_PostBlurEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...


}

