#include "Chunked.h"

#include "HAL/UnrealMemory.h"

void UChunked::Init(
  uint32_t inId,
  int64_t TotalSize,
  int32_t inNumChunks,
  int32_t inFullChunkSize
) {
  id = inId;
  Data.SetNum(TotalSize);
  NumChunks = inNumChunks;
  FullChunkSize = inFullChunkSize;
}

void UChunked::AddChunk(TArray<uint8_t>& Chunk, int32_t ChunkNum) {
  if (HasChunk(ChunkNum)) return;

  FMemory::Memcpy(
    Data.GetData() + ChunkNum * FullChunkSize,
    Chunk.GetData(),
    Chunk.Num()
  );

  MarkChunkReceived(ChunkNum);
  if (AllChunksReceived()) Finish();
}

void UChunked::MarkChunkReceived(int32_t ChunkNum) {
  ReceivedChunks.Add(ChunkNum);
}

bool UChunked::HasChunk(int32_t ChunkNum) {
  return ReceivedChunks.Contains(ChunkNum);
}

bool UChunked::AllChunksReceived() {
  return ReceivedChunks.Num() == NumChunks;
}

void UChunked::Finish() {
  UE_LOG(LogTemp, Warning, TEXT("osc: Chunked %d complete!"), id);
}