// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GHCharacter.generated.h"

class AGrapplePoint;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class GRAPPLINGHOOKSYSTEM_API AGHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGHCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USpringArmComponent* SpringArmComp;

	AGrapplePoint* GrapplePointRef;

	AActor* DetectedActor;
	AGHCharacter* GHCharacter;

	TArray<TEnumAsByte<enum EObjectTypeQuery>> objectTypes;

	float DetectionRadius;
	float GrappleThrowDistance;
	float HighestDotProduct;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void BeginJump();

	UFUNCTION(BlueprintCallable, Category="GrapplePoint")
	void CheckForGrapplePoints();

	UFUNCTION(BlueprintCallable, Category="GrapplePoint")
	void ActivateGrapplePoint();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint")
	void DeactivateGrapplePoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetDetectionRadius();
	float GetGrappleThrowDistance();
};
