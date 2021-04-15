// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplePoint.generated.h"

class AGHCharacter;
class UBoxComponent;
class UDetectionNode;
class UWidgetComponent;
class USphereComponent;
class UWidgetAnimation;

UCLASS()
class GRAPPLINGHOOKSYSTEM_API AGrapplePoint : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGrapplePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UWidgetComponent* DetectionNodeWidget;

	UDetectionNode* DetectionNode;

	FTimerHandle DectivateHandle;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	AGHCharacter* GHCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	UStaticMeshComponent* LandingZoneComp;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	UBoxComponent* DeactivateZoneComp;

	bool bIsUse;
	bool bIsActivate;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Activate(AGHCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Dectivate();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Rectivate();

	UFUNCTION(BlueprintCallable, Category = "Rope")
	void UseRope();

	UFUNCTION(BlueprintCallable, Category = "Rope")
	UStaticMeshComponent* GetLandingZone();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void CheckDistanceFromPlayer();

	UFUNCTION()
	void OnOverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
