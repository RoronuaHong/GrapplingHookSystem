// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GHCharacter.generated.h"

class USoundWave;
class AGrapplePoint;
class UCableComponent;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rope")
	UCableComponent* RopeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rope")
	UStaticMeshComponent* KunaiComp;

	UPROPERTY()
	USoundWave* GrappleWave;

	UPROPERTY()
	USoundWave* JumpWave;


	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* GroupLenCurve;

	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* AirLenCurve;


	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* GroupPosCurve;

	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* AirPosCurve;


	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* GroupSpeedCurve;

	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* AirSpeedCurve;


	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* GroupHeightOffsetCurve;

	UPROPERTY(EditDefaultsOnly, Category=Curve)
	UCurveFloat* AirHeightOffsetCurve;

	AGrapplePoint* GrapplePointRef;
	AGrapplePoint* CurrentGrapplePoint;

	AActor* DetectedActor;
	AGHCharacter* GHCharacter;

	FVector StartPos;
	FVector EndPos;
	FVector GrapplingDestination;

	UAnimMontage* GrappleAirAnim;
	UAnimMontage* GrappleGroundAnim;

	TArray<TEnumAsByte<enum EObjectTypeQuery>> objectTypes;

	float CurrentDistance;
	float DetectionRadius;
	float GrappleThrowDistance;
	float HighestDotProduct;
	float RopeBaseLength;
	float CurrentLength;
	float CurrentLen;
	float KunaiAlpha;
	
	bool bIsInGrapplingAnimation;
	bool bIsMovingWithGrapple;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void BeginJump();

	UFUNCTION(BlueprintCallable, Category="GrapplePoint")
	void CheckForGrapplePoints();

	UFUNCTION(BlueprintCallable, Category="GrapplePoint")
	void ActivateGrapplePoint();

	UFUNCTION(BlueprintCallable, Category="GrapplePoint")
	void DeactivateGrapplePoint();

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	void ThrowGrapple();

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	void GrapplingMovement();

	void MoveRope();
	void SettingRopeParam();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="Grapple")
	void PlayJumpWave();

	UFUNCTION(BlueprintCallable, Category="Grapple")
	void PlayGrappleWave();

	UFUNCTION(BlueprintCallable, Category="Grapple")
	void StartGrapplingMovement();

	UFUNCTION(BlueprintCallable, Category="Grapple")
	void RopeVisibility(bool visible);

	UFUNCTION(BlueprintCallable, Category="Grapple")
	void ResetMovement();

	float GetDetectionRadius();
	float GetGrappleThrowDistance();
};
