#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chunked.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCompleteDelegate, uint32_t /* id */, TArray<uint8_t>& /* Data */);

UCLASS()
class RXCHUNK_API UChunked : public UObject {
	GENERATED_BODY()
	
public:
  uint32_t id;
  int32_t NumChunks;
  TArray<uint8_t> Data;
  TSet<uint32_t> ReceivedChunks;

  void Init(uint32_t inId, size_t TotalSize, int32_t inNumChunks);
  void AddChunk(TArray<uint8_t>& Chunk, int32_t ChunkNum, size_t ChunkSize);
  bool HasChunk(int32_t ChunkNum);
  void MarkChunkReceived(int32_t ChunkNum);
  bool AllChunksReceived();

  FOnCompleteDelegate OnComplete;
};
