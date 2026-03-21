// Fill out your copyright notice in the Description page of Project Settings.


#include "C_VAT.h"
#include "Engine/Engine.h"
#include "NiagaraComponent.h"
#include "NiagaraActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AC_VAT::AC_VAT()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Debug
	UE_LOG(LogTemp, Log, TEXT("Create C_VAT"));

	//初始化Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	
	///Script/Engine.StaticMesh'/Game/SongGuFu_VFXContent/Ability/ID11_TencentTest/Art/VAT/VAT_Ground_5/vertex_animation_textures1_mesh.vertex_animation_textures1_mesh'
	ConstructorHelpers::FObjectFinder<UStaticMesh> InMesh(
		TEXT("/Game/SongGuFu_VFXContent/Ability/ID11_TencentTest/Art/VAT/VAT_Ground_5/vertex_animation_textures1_mesh.vertex_animation_textures1_mesh")
	);

	MeshComponent->SetStaticMesh(InMesh.Object);
	
	
	//初始化Material
	ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> InMaterial(
		TEXT("/Game/SongGuFu_VFXContent/Ability/ID11_TencentTest/Material/Niagara/MaterialSource/M_AID11_VAT_Inst.M_AID11_VAT_Inst")
	);

	//初始化材质
	AllMaterial.SetNum(1);
	AllMaterial[0] = FStMaterialSlot(0, InMaterial.Object);

	
}



// Called when the game starts or when spawned
void AC_VAT::BeginPlay()
{
	Super::BeginPlay();

	//初始化Mesh
	
	if (InVATMesh)
	{
		UE_LOG(LogTemp, Log, TEXT("Replacement Mesh"));
		MeshComponent->SetStaticMesh(InVATMesh);
	}
	//初始化MeshTransform
	InitialPosition = GetActorLocation() + OffsetPosition;
	MeshComponent->SetWorldLocation(InitialPosition);
	
	MeshComponent->SetRelativeScale3D(Scale);




	//将材质添加至Mesh,并初始化材质实例
	for (FStMaterialSlot& Slot : AllMaterial)
	{
		if (!Slot.Material) continue;

		UMaterialInstanceDynamic* DynMat =
			UMaterialInstanceDynamic::Create(Slot.Material, this);

		if (!DynMat) continue;
		DynamicMaterials.Add(DynMat);

		if (MeshComponent)
		{
			MeshComponent->SetMaterial(Slot.Index, DynMat);
		}
	}

	//注册Niagara
	ANiagaraActor* OwnerNiagara = Cast<ANiagaraActor>(GetOwner());

	if (OwnerNiagara)
	{
		//UNiagaraComponent* NiagaraComp = OwnerNiagara->GetNiagaraComponent();

		OwnerNiagara->GetNiagaraComponent()->SetVariableObject(TEXT("User.CallBackObject"), this);

	}


}

// Called every frame
void AC_VAT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//核心逻辑实现
void AC_VAT::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset)
{
	if (Data.Num() == 0) return;

	if (Data[0].Velocity[0] < 1) { Destroy(); }

	MeshComponent->SetWorldLocation(InitialPosition + Data[0].Position);
	MeshComponent->SetRelativeScale3D(Scale* FVector(1,1,Data[0].Velocity[1]));

	for (UMaterialInstanceDynamic* Mat : DynamicMaterials)
	{
		if (!Mat) continue;
		
		/*
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Size: %s"), *Mat->GetName())
		);
		*/

		
		Mat->SetScalarParameterValue(TEXT("Time"), Data[0].Size);
	}
	
}
