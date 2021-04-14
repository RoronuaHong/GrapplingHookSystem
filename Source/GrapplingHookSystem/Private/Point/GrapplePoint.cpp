// Fill out your copyright notice in the Description page of Project Settings.


#include "Point/GrapplePoint.h"
#include "Components/Image.h"
#include "UI/DetectionNode.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/GHCharacter.h"
#include "kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
AGrapplePoint::AGrapplePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComp"));
	LandingZoneComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LandingZoneComp"));
	DeactivateZoneComp = CreateDefaultSubobject<UBoxComponent>(TEXT("DeactivateZoneComp"));
	DetectionNodeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionNodeWidget"));

	static ConstructorHelpers::FClassFinder<UUserWidget> DetectionNodeObj(TEXT("/Game/UI/WB_DetactionNode.WB_DetactionNode_C"));

	if(DetectionNodeObj.Succeeded()) {
		DetectionNodeWidget->SetWidgetClass(DetectionNodeObj.Class);
	}

	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	DetectionNodeWidget->SetWidgetSpace(EWidgetSpace::Screen);


	RootComponent = SphereComp;
	DetectionNodeWidget->SetupAttachment(RootComponent);
	LandingZoneComp->SetupAttachment(RootComponent);

	bIsActivate = false;
}

// Called when the game starts or when spawned
void AGrapplePoint::BeginPlay()
{
	Super::BeginPlay();

	// 需要使用到 GetUserWidgetObject()
	DetectionNode = Cast<UDetectionNode>(DetectionNodeWidget->GetUserWidgetObject());
	DetectionNode->SetVisibility(ESlateVisibility::Hidden);
}

// Called every frame
void AGrapplePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsActivate) {
		CheckDistanceFromPlayer();
	}
}

void AGrapplePoint::Activate(AGHCharacter* Character) {
	if(Character) {
		bIsActivate = true;

		DetectionNode->SetVisibility(ESlateVisibility::Visible);

		GHCharacter = Character;
	}
}

void AGrapplePoint::Dectivate() {
	bIsActivate = false;

	DetectionNode->SetVisibility(ESlateVisibility::Hidden);
}

void AGrapplePoint::CheckDistanceFromPlayer() {
	float OutRangeA = 10.0f;
	float OutRangeB = 80.0f;
	float InRangeA = GHCharacter->GetDetectionRadius();
	float InRangeB = GHCharacter->GetGrappleThrowDistance();

	float DistanceFromPlayer = UKismetMathLibrary::MapRangeClamped((GHCharacter->GetActorLocation() - GetActorLocation()).Size(), 
		InRangeA, InRangeB, OutRangeA, OutRangeB);

	DetectionNode->GetFilling()->SetBrushSize(FVector2D(DistanceFromPlayer, DistanceFromPlayer));

	FColor CurrentColor;

	if(DistanceFromPlayer == OutRangeB) {
		CurrentColor = FColor::Blue;
	} else {
		CurrentColor = FColor::White;
	}

	DetectionNode->GetFilling()->SetColorAndOpacity(CurrentColor);
}
