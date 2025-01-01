#pragma once

#include "CoreMinimal.h"
#include "Chunked.h"
#include "ChunkedTest.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChunkedTestCompleteDelegate, uint32_t /* id */, TArray<uint8_t>& /* Data */);

UCLASS()
class RXCHUNK_API UChunkedTest : public UChunked {
	GENERATED_BODY()
	
public:
  FOnChunkedTestCompleteDelegate OnComplete;

private:
  void Finish() override;
};
