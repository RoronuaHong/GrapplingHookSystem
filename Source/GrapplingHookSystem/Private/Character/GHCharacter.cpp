// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GHCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Point/GrapplePoint.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "CableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimMontage.h"

// Sets default values
AGHCharacter::AGHCharacter() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	RopeComp = CreateDefaultSubobject<UCableComponent>(TEXT("RopeComp"));
	KunaiComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KunaiComp"));

	RopeComp->SetAttachEndTo(this, FName(KunaiComp->GetName()));

	FRotator Rotation;

	Rotation.Yaw = -90.0f;
	Rotation.Pitch = 0.0f;
	Rotation.Roll = 0.0f;

	KunaiComp->SetWorldRotation(Rotation);

	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);
	RopeComp->SetupAttachment(GetMesh(), TEXT("hand_l"));
	KunaiComp->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	DetectionRadius = 2000.0f;
	GrappleThrowDistance = 1200.0f;

	// Get Grapple Wave.
	static ConstructorHelpers::FObjectFinder<USoundWave> GrappleWaveObj(TEXT("SoundWave'/Game/Sounds/A_Grapple.A_Grapple'"));

	if(GrappleWaveObj.Object) {
		GrappleWave = GrappleWaveObj.Object;
	}

	// Get Sound Wave.
	static ConstructorHelpers::FObjectFinder<USoundWave> JumpWaveObj(TEXT("SoundWave'/Game/Sounds/A_Jump.A_Jump'"));

	if(JumpWaveObj.Object) {
		JumpWave = JumpWaveObj.Object;
	}

	// reference GrappleGroundAnim
	ConstructorHelpers::FObjectFinder<UAnimMontage> GrappleGroundAnimObj(TEXT("AnimMontage'/Game/Animations/AM_GrappleGround.AM_GrappleGround'"));

	if(GrappleGroundAnimObj.Object) {
		GrappleGroundAnim = GrappleGroundAnimObj.Object;
	}

	// reference GrappleAirAnim
	ConstructorHelpers::FObjectFinder<UAnimMontage> GrappleAirAnimObj(TEXT("AnimMontage'/Game/Animations/AM_GrappleAir.AM_GrappleAir'"));

	if(GrappleGroundAnimObj.Object) {
		GrappleAirAnim = GrappleAirAnimObj.Object;
	}

	// reference GroupRopeLength
	ConstructorHelpers::FObjectFinder<UCurveFloat> GroupLenCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_RopeLengthGround_Float.Curve_RopeLengthGround_Float'"));

	if(GroupLenCurveObj.Object) {
		GroupLenCurve = GroupLenCurveObj.Object;
	}

	// reference AirRopeLength
	ConstructorHelpers::FObjectFinder<UCurveFloat> AirLenCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_RopeLengthGround_Float.Curve_RopeLengthGround_Float'"));

	if(AirLenCurveObj.Object) {
		AirLenCurve = AirLenCurveObj.Object;
	}


	// reference GroupRopeLength
	ConstructorHelpers::FObjectFinder<UCurveFloat> GroupPosCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_RopePositionGround_Float.Curve_RopePositionGround_Float'"));

	if(GroupPosCurveObj.Object) {
		GroupPosCurve = GroupPosCurveObj.Object;
	}

	// reference AirRopeLength
	ConstructorHelpers::FObjectFinder<UCurveFloat> AirPosCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_RopePositionAir_Float.Curve_RopePositionAir_Float'"));

	if(AirPosCurveObj.Object) {
		AirPosCurve = AirPosCurveObj.Object;
	}


	// reference GroupRopeSpeed
	ConstructorHelpers::FObjectFinder<UCurveFloat> GroupSpeedCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_GrappleSpeedGround_Float.Curve_GrappleSpeedGround_Float'"));

	if(GroupSpeedCurveObj.Object) {
		GroupSpeedCurve = GroupSpeedCurveObj.Object;
	}

	// reference AirRopeSpeed
	ConstructorHelpers::FObjectFinder<UCurveFloat> AirSpeedCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_GrappleSpeedAir_Float.Curve_GrappleSpeedAir_Float'"));

	if(AirSpeedCurveObj.Object) {
		AirSpeedCurve = AirSpeedCurveObj.Object;
	}


	// reference GroupRopeHeightOffset
	ConstructorHelpers::FObjectFinder<UCurveFloat> GroupHeightOffsetCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_HeightOffsetGround_Float.Curve_HeightOffsetGround_Float'"));
	
	if(GroupHeightOffsetCurveObj.Object) {
		GroupHeightOffsetCurve = GroupHeightOffsetCurveObj.Object;
	}

	// reference AirRopeHeightOffset
	ConstructorHelpers::FObjectFinder<UCurveFloat> AirHeightOffsetCurveObj(TEXT("CurveFloat'/Game/Animations/Curve_HeightOffsetAir_Float.Curve_HeightOffsetAir_Float'"));

	if(AirHeightOffsetCurveObj.Object) {
		AirHeightOffsetCurve = AirSpeedCurveObj.Object;
	}
}

// Called when the game starts or when spawned
void AGHCharacter::BeginPlay() {
	Super::BeginPlay();

	SettingRopeParam();
	
	GHCharacter = Cast<AGHCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	GHCharacter->GetCharacterMovement()->GravityScale = 2.2f;

	StartPos = GHCharacter->GetActorLocation();
}

// Called every frame
void AGHCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	CheckForGrapplePoints();

	if(bIsInGrapplingAnimation) {
		MoveRope();
	}

	if(bIsMovingWithGrapple) {
		GrapplingMovement();
	}
}

void AGHCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector() * Value);
}

void AGHCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector() * Value);
}

void AGHCharacter::BeginCrouch() {
	Crouch();

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AGHCharacter::EndCrouch() {
	UnCrouch();
}

void AGHCharacter::BeginJump() {
	Jump();
}

// Called to bind functionality to input
void AGHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGHCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGHCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AGHCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AGHCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGHCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &AGHCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGHCharacter::BeginJump);

	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AGHCharacter::ThrowGrapple);
}

void AGHCharacter::CheckForGrapplePoints() {
	FCollisionShape MySphere = FCollisionShape::MakeSphere(DetectionRadius);
	FCollisionObjectQueryParams COQP;
	TArray<FHitResult> OutHits;
	FHitResult OutHit;
	TArray<AActor*> IgnoreArray;

	IgnoreArray.Add(GHCharacter);
	IgnoreArray.Add(GetOwner());

	// 使用 SweepMultiByObjectType 进行检测
	COQP.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	const bool bIsFound = GetWorld()->SweepMultiByObjectType(OutHits, GHCharacter->GetActorLocation(), GHCharacter->GetActorLocation(), FQuat::Identity, COQP, MySphere);

	DeactivateGrapplePoint();

	if(bIsFound) {
		HighestDotProduct = 0.7f;

		for(const FHitResult Item : OutHits) {
			if(Item.GetActor()) {
				const FVector ForwardResult = CameraComp->GetForwardVector();
				const FVector CameraResult = (Item.GetActor()->GetActorLocation() - CameraComp->GetComponentLocation()).GetSafeNormal();

				float DotProduct = FVector::DotProduct(ForwardResult, CameraResult);

				if(DotProduct > HighestDotProduct) {
					HighestDotProduct = DotProduct;
					DetectedActor = Item.GetActor();
				}
			}
		}

		if(DetectedActor) {
			DeactivateGrapplePoint();

			GrapplePointRef = Cast<AGrapplePoint>(DetectedActor);

			if(GrapplePointRef) {
				ActivateGrapplePoint();
			}
		}
	}
}

void AGHCharacter::ActivateGrapplePoint() {
	FHitResult OutHit;

	const TArray<AActor*> IgnoreArray;
	FCollisionQueryParams TraceParams;
	FCollisionObjectQueryParams COQP;

	FVector StartVector = CameraComp->GetComponentLocation();
	FVector EndVector = DetectedActor->GetActorLocation();

	COQP.AddObjectTypesToQuery(ECC_Visibility);

	// 实现看得见的点亮功能
	const bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartVector, EndVector, ECC_Visibility, TraceParams);
	//DrawDebugLine(GetWorld(), StartVector, EndVector, FColor::Blue, false, 1.0f);

	if(bIsHit) {
		if(DetectedActor == OutHit.GetActor()) {
			RopeComp->SetAttachEndTo(this, FName(KunaiComp->GetName()));

			SettingRopeParam();

			GrapplePointRef->Activate(this);
		}
	}
}

void AGHCharacter::DeactivateGrapplePoint() {
	if(GrapplePointRef) {
		GrapplePointRef->Dectivate();
		GrapplePointRef = nullptr;
	}
}

void AGHCharacter::PlayGrappleWave() {
	UGameplayStatics::PlaySound2D(this, GrappleWave);
}

void AGHCharacter::PlayJumpWave() {
	UGameplayStatics::PlaySound2D(this, JumpWave);
}

void AGHCharacter::ThrowGrapple() {
	if(GrapplePointRef) {
		CurrentDistance = GrapplePointRef->GetDistanceTo(GHCharacter);

		if(CurrentDistance <= GrappleThrowDistance && CurrentGrapplePoint != GrapplePointRef) {
			if(bIsMovingWithGrapple) {
				// TODO:
				//GrapplingDestination
			} else {
				CurrentGrapplePoint = GrapplePointRef;

				bIsInGrapplingAnimation = true;
				bIsMovingWithGrapple = false;

				GrapplingDestination = CurrentGrapplePoint->GetLandingZone()->GetComponentLocation() + FVector(0.0f, 0.0f, 110.0f);

				// TODO: 角色转向.
				//FRotator NewFVector = UKismetMathLibrary::FindLookAtRotation(GHCharacter->GetActorLocation(), GrapplingDestination);
				//GHCharacter->SetActorRotation(FRotator(0.0f, 0.0f, NewFVector.Yaw));

				RopeBaseLength = (GHCharacter->GetActorLocation() - GrapplingDestination).Size();

				GrapplePointRef->UseRope();

				if(GHCharacter->GetCharacterMovement()->IsFalling()) {
					GHCharacter->PlayAnimMontage(GrappleAirAnim);
				} else {
					GHCharacter->PlayAnimMontage(GrappleGroundAnim);
					GHCharacter->GetCharacterMovement()->DisableMovement();
				}
			}
		}
	}
}

void AGHCharacter::GrapplingMovement() {
	USkeletalMeshComponent* CMesh = GetMesh();

	if(CMesh) {
		UAnimInstance * AnimInst = CMesh->GetAnimInstance();

		if(AnimInst) {
			float SpeedAlpha = GroupSpeedCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));
			float HeightOffsetAlpha = GroupHeightOffsetCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));

			if(AnimInst->GetCurrentActiveMontage() != GrappleGroundAnim) {
				SpeedAlpha = AirSpeedCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));
				HeightOffsetAlpha = AirHeightOffsetCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));
			}

			FVector ReturnVector = FMath::Lerp(StartPos, GrapplingDestination, SpeedAlpha);

			ReturnVector.Z = ReturnVector.Z + HeightOffsetAlpha;

			GHCharacter->SetActorLocation(ReturnVector);
		}
	}
}

void AGHCharacter::StartGrapplingMovement()
{
	if(GHCharacter) {
		PlayJumpWave();

		GHCharacter->GetCharacterMovement()->StopMovementImmediately();
		GHCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

		StartPos = GHCharacter->GetActorLocation();

		bIsMovingWithGrapple = true;
	}
}

void AGHCharacter::RopeVisibility(bool visible) {
	KunaiComp->SetVisibility(visible);
	RopeComp->SetVisibility(visible);
}

void AGHCharacter::ResetMovement() {
	bIsMovingWithGrapple = false;
	bIsInGrapplingAnimation = false;

	CurrentGrapplePoint = nullptr;

	GHCharacter->GetCharacterMovement()->GravityScale = 2.2f;
	GHCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AGHCharacter::MoveRope()
{
	USkeletalMeshComponent* CMesh = GetMesh();
	
	if(CMesh) {
		UAnimInstance* AnimInst = CMesh->GetAnimInstance();

		if(AnimInst) {
			CurrentLen = GroupLenCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));
			KunaiAlpha = GroupPosCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));

			if(AnimInst->GetCurrentActiveMontage() != GrappleGroundAnim) {
				CurrentLen = AirLenCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));
				KunaiAlpha = AirPosCurve->GetFloatValue(AnimInst->Montage_GetPosition(AnimInst->GetCurrentActiveMontage()));
			}

			RopeComp->CableLength = CurrentLen;

			FVector CurrentVector = FMath::Lerp(CMesh->GetSocketLocation(TEXT("hand_l")), CurrentGrapplePoint->GetActorLocation(), KunaiAlpha);

			KunaiComp->SetWorldLocation(CurrentVector);
		}
	}
}

void AGHCharacter::SettingRopeParam() {
	RopeComp->CableLength = 20.f;
	RopeComp->CableWidth = 3.5f;
	RopeComp->NumSides = 16;
	RopeComp->TileMaterial = 8.0f;
	RopeComp->NumSegments = 100.f;
	RopeComp->SolverIterations = 40;
}

float AGHCharacter::GetDetectionRadius() {
	return DetectionRadius;
}

float AGHCharacter::GetGrappleThrowDistance() {
	return GrappleThrowDistance;
}
