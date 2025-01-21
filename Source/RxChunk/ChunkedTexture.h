#pragma once

#include "CoreMinimal.h"
#include "Chunked.h"
#include "ChunkedTexture.generated.h"

class UTexture2D;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChunkedTextureCompleteDelegate, UChunkedTexture* /* this */);

UCLASS()
class RXCHUNK_API UChunkedTexture : public UChunked {
	GENERATED_BODY()
	
public:
  void SetDimensions(int32_t inWidth, int32_t inHeight);
  void SetCompressed(bool inbCompressed);
  int32_t Width{0}, Height{0};

  FOnChunkedTextureCompleteDelegate OnComplete;

  UTexture2D* MakeTexture2D();

private:
  bool bCompressed{false};

  void Finish() override;
  void Decompress();
};