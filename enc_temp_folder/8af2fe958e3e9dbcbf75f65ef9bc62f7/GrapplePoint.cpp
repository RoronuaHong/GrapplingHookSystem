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
	DeactivateZoneComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	DetectionNodeWidget->SetWidgetSpace(EWidgetSpace::Screen);

	RootComponent = SphereComp;

	DetectionNodeWidget->SetupAttachment(RootComponent);
	LandingZoneComp->SetupAttachment(RootComponent);
	DeactivateZoneComp->SetupAttachment(RootComponent);

	bIsActivate = false;
}

// Called when the game starts or when spawned
void AGrapplePoint::BeginPlay()
{
	Super::BeginPlay();

	// 需要使用到 GetUserWidgetObject()
	DetectionNode = Cast<UDetectionNode>(DetectionNodeWidget->GetUserWidgetObject());
	DetectionNode->SetVisibility(ESlateVisibility::Hidden);

	DeactivateZoneComp->OnComponentBeginOverlap.AddDynamic(this, &AGrapplePoint::OnOverlapBegins);
	DeactivateZoneComp->OnComponentEndOverlap.AddDynamic(this, &AGrapplePoint::OnOverlapEnd);
}

// Called every frame
void AGrapplePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsActivate && !bIsUse) {
		CheckDistanceFromPlayer();
	}
}

void AGrapplePoint::Activate(AGHCharacter* Character) {
	if(!bIsUse) {
		if(Character) {
			bIsActivate = true;
			DetectionNode->SetVisibility(ESlateVisibility::Visible);

			GHCharacter = Character;
		}
	}
}

void AGrapplePoint::Dectivate() {
	if(!bIsUse) {
		bIsActivate = false;

		DetectionNode->SetVisibility(ESlateVisibility::Hidden);

		DetectionNode->StopAllAnimations();
	}
}

void AGrapplePoint::Rectivate()
{
	if(bIsUse) {
		bIsUse = false;

		// FIXME: 采用回放的方式恢复原形.
		DetectionNode->SetVisibility(ESlateVisibility::Hidden);
		DetectionNode->PlayAnimation(DetectionNode->GetNodeUse(), 0.0f, 2, EUMGSequencePlayMode::Reverse, 1000.f, false);

		GetWorldTimerManager().ClearTimer(DectivateHandle);
	}
}

void AGrapplePoint::UseRope()
{
	bIsUse = true;

	if(DetectionNode) {
		DetectionNode->PlayAnimation(DetectionNode->GetNodeUse(), 0.0f, 1, EUMGSequencePlayMode::Forward, 1.f, true);
		//DetectionNode->SetVisibility(ESlateVisibility::Hidden);
	}

	GetWorldTimerManager().SetTimer(DectivateHandle, this, &AGrapplePoint::Rectivate, 3.0f, false);
}

UStaticMeshComponent* AGrapplePoint::GetLandingZone()
{
	return LandingZoneComp;
}

void AGrapplePoint::CheckDistanceFromPlayer() {
	if(GHCharacter) {
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
}

void AGrapplePoint::OnOverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { 
	AGHCharacter* MyGHCharacter = Cast<AGHCharacter>(OtherActor);

	if(MyGHCharacter) {
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void AGrapplePoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AGHCharacter* MyGHCharacter = Cast<AGHCharacter>(OtherActor);

	if(MyGHCharacter) {
		SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}
