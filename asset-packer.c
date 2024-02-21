#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#pragma pack(1)
typedef struct bmp_header
{
  uint16_t Signature;
  uint32_t FileSize;
  uint32_t Reserved;
  uint32_t DataOffset;
} bmp_header;
#pragma pack(pop)

#pragma pack(1)
typedef struct bmp_info_header
{
  uint32_t Size;
  uint32_t Width;
  uint32_t Height;
  uint16_t Planes;
  uint16_t BitsPerPixel;
  uint32_t Compression;
} bmp_info_header;
#pragma pack(pop)

#pragma pack(1)
typedef struct hbm_header
{
  uint32_t Width;
  uint32_t Height;
} hbm_header;
#pragma pack(pop)

int main(int FileCount, char **Files)
{
	FILE *FileP;
	for(int FileIndex = 1; FileIndex < FileCount; ++FileIndex)
	{
    char *FileName = Files[FileIndex];
    int FileNameLength = strlen(FileName);
    
    printf("Opening file %s...\n", FileName);
    FileP = fopen(FileName, "r");
    if(FileP == 0)
    {
      printf("Cannot find file %s\n", FileName);
      continue;
    }
    
    fseek(FileP, 0L, SEEK_END);
    size_t Size = ftell(FileP);
    rewind(FileP);
    
    uint8_t *Data = malloc(Size);
    fread(Data, 1, Size, FileP);
    fclose(FileP);
    
    int ScanIndex = 0;
    bmp_header *Header = (bmp_header *)(Data + ScanIndex);
    ScanIndex += sizeof(bmp_header);
    printf("File Size: %d\n", Header->FileSize);
    
    bmp_info_header *InfoHeader = (bmp_info_header *)(Data + ScanIndex);
    printf("Width: %d\n", InfoHeader->Width);
    printf("Height: %d\n", InfoHeader->Height);
    printf("Compression: %d\n", InfoHeader->Compression);
    
    uint32_t *PixelData = (uint32_t *)(Data + Header->DataOffset);
    for(int PixelIndex = 0; PixelIndex < InfoHeader->Width*InfoHeader->Height; ++PixelIndex)
    {
	    uint32_t *Pixel = &PixelData[PixelIndex];
	    // NOTE: Format is ABGR, switch to ARGB
	    uint8_t A = (uint8_t)(*Pixel >> 24);
	    uint8_t B = (uint8_t)(*Pixel >> 16);
	    uint8_t G = (uint8_t)(*Pixel >> 8);
	    uint8_t R = (uint8_t)(*Pixel >> 0);
	    *Pixel = ((uint32_t)A << 24) | ((uint32_t)R << 16) | ((uint32_t)G << 8) | ((uint32_t)B << 0);
    }

    char Extension[] = "hmp";
    strcpy(FileName + (FileNameLength - 3), Extension);
    
    FileP = fopen(FileName, "wb");
    hbm_header HBMHeader = {InfoHeader->Width, InfoHeader->Height};
    fwrite(&HBMHeader, sizeof(hbm_header), 1, FileP);
    fwrite(PixelData, InfoHeader->Width*InfoHeader->Height*4, 1, FileP);
    fclose(FileP);
    
    free(Data);
	}
  
	return(0);
}
