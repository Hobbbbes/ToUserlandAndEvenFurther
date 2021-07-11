#include "paging.h"
#include <stdint.h>
#include <efilib.h>
typedef struct _PageStructureIndizes{
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
} PageStructureIndizes;

PageStructureIndizes getIndizes(uint64_t virtualAddr){
    PageStructureIndizes res;
    virtualAddr >>= 12;
    res.P_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PT_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PD_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PDP_i = virtualAddr & 0x1ff;
    return res;
}

uint64_t setaddr(uint64_t paddr){
    uint64_t value;
    paddr &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (paddr << 12);
    return value + 1; //Set first bit, Present Flag
}
uint64_t getaddr(uint64_t entry){
    return (entry & 0x000ffffffffff000) >> 12;
}
typedef uint64_t PageDirEntry;
void MapMemory(void* PLM4, uint64_t paddr, uint64_t vaddr){
    PageStructureIndizes indizes = getIndizes(vaddr);
    PageDirEntry pde = ((PageDirEntry*)PLM4)[indizes.PDP_i];
    PageDirEntry* pagetable = NULL;
    if(pde == 0){
        ST->BootServices->AllocatePages(AllocateAnyPages,EfiConventionalMemory,1,&pde);
        ST->BootServices->SetMem(pde,0x1000,0x00);
        ((PageDirEntry*)PLM4)[indizes.PDP_i] = setaddr(pde >> 12);
        pagetable = pde;
    } else {
        pagetable = getaddr(pde) << 12;
    }
    pde = pagetable[indizes.PD_i];
    pagetable = NULL;
    if(pde == 0){
        ST->BootServices->AllocatePages(AllocateAnyPages,EfiConventionalMemory,1,&pde);
        ST->BootServices->SetMem(pde,0x1000,0x00);
        ((PageDirEntry*)PLM4)[indizes.PDP_i] = setaddr(pde >> 12);
        pagetable = pde;
    } else {
        pagetable = getaddr(pde) << 12;
    }
    pde = pagetable[indizes.PT_i];
    pagetable = NULL;
    if(pde == 0){
        ST->BootServices->AllocatePages(AllocateAnyPages,EfiConventionalMemory,1,&pde);
        ST->BootServices->SetMem(pde,0x1000,0x00);
        ((PageDirEntry*)PLM4)[indizes.PDP_i] = setaddr(pde >> 12);
        pagetable = pde;
    } else {
        pagetable = getaddr(pde) << 12;
    }
    pde = pagetable[indizes.P_i];
    pagetable = NULL;
    pagetable[indizes.P_i] = setaddr(paddr);
}

int MapEfiMemoryMap(UINTN MapSize, UINTN DescriptorSize, EFI_MEMORY_DESCRIPTOR* Map, void* PLM4){
    UINTN map_ptr_value = (UINTN)Map;
	for(UINTN i = 0; i < MapSize / DescriptorSize; i++, map_ptr_value += DescriptorSize){
		Map = (EFI_MEMORY_DESCRIPTOR*)map_ptr_value;
		MapMemory(PLM4,Map->PhysicalStart,Map->PhysicalStart);
	}
}