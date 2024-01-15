// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleSlot.h"

// Sets default values
AObstacleSlot::AObstacleSlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Obstacle = CreateDefaultSubobject<UBoxComponent>(TEXT("Obstacle"));
	Obstacle->SetupAttachment(RootComponent);
	Obstacle->SetBoxExtent(FVector(50, 10, 2));
	Obstacle->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultSlotMesh(TEXT("/Engine/BasicShapes/Plane"));
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(Obstacle);
	Plane->SetStaticMesh(DefaultSlotMesh.Object);

	SetState(GS_Default);
}

void AObstacleSlot::SetState(EBoxState NewState)
{
	ObstacleState = NewState;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		DefaultSlotMaterial(TEXT("/Script/Engine.Material'/Game/Materials/M_Default_Obstacle.M_Default_Obstacle'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		DefaultSlotMaterial_A(TEXT("/Script/Engine.Material'/Game/Materials/M_Acceptable.M_Acceptable'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		DefaultSlotMaterial_U(TEXT("/Script/Engine.Material'/Game/Materials/M_Unacceptable.M_Unacceptable'"));

	switch (NewState)
	{
	case GS_Default:
		Plane->SetMaterial(0, DefaultSlotMaterial.Object);
		break;
	case GS_Acceptable:
		Plane->SetMaterial(0, DefaultSlotMaterial_A.Object);
		break;
	case GS_Unacceptable:
		Plane->SetMaterial(0, DefaultSlotMaterial_U.Object);
		break;

	}
}

EBoxState AObstacleSlot::GetState() const
{
	return EBoxState();
}

// Called when the game starts or when spawned
void AObstacleSlot::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AObstacleSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

