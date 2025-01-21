#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Faced.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UTexture2D;
class UDynamicTextureComponent;
class UChunkedTexture;

UCLASS()
class RXCHUNK_API AFaced : public AActor {
	GENERATED_BODY()
	
public:	
	AFaced();

protected:
	virtual void BeginPlay() override;

public:	
  void SetSize(float Width, float Height);
  void SetBaseTexture(UTexture2D* Texture);
  void SetOverlayData(UChunkedTexture* ChunkedTexture);

private:
  float ScalingFactor{10.f};

  UPROPERTY()
  UTexture2D* BaseTexture;
  UPROPERTY()
  UDynamicTextureComponent* OverlayTextureComponent;

  const wchar_t* MeshRef{TEXT(
    "/Script/Engine.StaticMesh'/Game/meshes/unit_cube.unit_cube'"
  )};
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* StaticMeshComponent;
  UPROPERTY()
  UStaticMesh* StaticMesh;

  const wchar_t* BaseMaterialRef{TEXT(
    "/Script/Engine.Material'/Game/materials/generic_base.generic_base'"
  )};
  UPROPERTY()
  UMaterialInstanceDynamic* BaseMaterialInstance;
  UPROPERTY()
  UMaterialInterface* BaseMaterialInterface;

  const wchar_t* FaceMaterialRef{TEXT(
    "/Script/Engine.Material'/Game/materials/texture_face_with_overlay.texture_face_with_overlay'"
  )};
  UPROPERTY()
  UMaterialInstanceDynamic* FaceMaterialInstance;
  UPROPERTY()
  UMaterialInterface* FaceMaterialInterface;
};
