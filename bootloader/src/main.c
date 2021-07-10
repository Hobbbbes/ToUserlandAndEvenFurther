#include <efi.h>
#include <efilib.h>
#include <elf.h>


typedef struct{
	void * BaseAddress;
	UINTN BufferSize;
	UINTN Width;
	UINTN Height;
	UINTN PixelsPerScanLine;
} FrameBuffer;

#define PSF_MAGIC0 0x36
#define PSF_MAGIC1 0x04

typedef struct{
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
}PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
}PSF1_FONT;

EFI_FILE * LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE * SystemTable){
	EFI_FILE * loadedFile;
	EFI_LOADED_IMAGE_PROTOCOL * LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle,&gEfiLoadedImageProtocolGuid,(void**) &LoadedImage);
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle,&gEfiSimpleFileSystemProtocolGuid,(void**)&FileSystem);
	if(Directory == NULL){
		FileSystem->OpenVolume(FileSystem,&Directory);
	}
	EFI_STATUS s = Directory->Open(Directory,&loadedFile,Path,EFI_FILE_MODE_READ,EFI_FILE_READ_ONLY);

	if(s != EFI_SUCCESS){
		return NULL;
	}
	return loadedFile;

}


PSF1_FONT* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE * SystemTable){
	EFI_FILE* font = LoadFile(Directory,Path,ImageHandle,SystemTable);
	if (font == NULL) return NULL;
	PSF1_HEADER* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font,&size,fontHeader);
	if(fontHeader->magic[0] != PSF_MAGIC0 || fontHeader->magic[1] != PSF_MAGIC1){
		return NULL;
	}
	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if(fontHeader->mode == 1){
		glyphBufferSize = fontHeader->charsize * 512;
	}
	void * glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData,glyphBufferSize,(void**)&glyphBuffer);
		font->Read(font,&glyphBufferSize,glyphBuffer);
	}
	PSF1_FONT* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData,sizeof(PSF1_FONT),(void**)&finishedFont);
	finishedFont->psf1_Header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;
}


FrameBuffer framebuffer;

FrameBuffer * InitGOP(){
	EFI_GUID gopGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL * gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol,3 , &gopGUID,NULL,(void**) &gop);

	if(EFI_ERROR(status)){
		Print(L"Failed to locate GOP\n");
		return NULL;
	} else {
		Print(L"GOP located\n");
	}
	
	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
	return &framebuffer;
}


int memcmp(const void * aptr, const void * bptr, const UINTN n){
	const unsigned char * a =aptr,* b = bptr;
	for(UINTN i = 0; i<n; i++){
		if(a[i] != b[i]){
			return 0;
		}
	}
	return 1;
}

typedef struct BootInfo
{
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	UINTN mMapSize;
	UINTN mMapDescriptorSize;
} BootInfo;

void print_memorymap(EFI_MEMORY_DESCRIPTOR* map, UINTN DescriptorSize,UINTN MapSize){
	UINTN map_ptr_value = (UINTN)map;
	for(UINTN i = 0; i < MapSize / DescriptorSize; i++, map_ptr_value += DescriptorSize){
		map = (EFI_MEMORY_DESCRIPTOR*)map_ptr_value;
		Print(L"Physical Start: %lx  Virtual Start: %lx Size: %ld\n",
		map->PhysicalStart,map->VirtualStart,map->NumberOfPages);
	}
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	
	InitializeLib(ImageHandle,SystemTable);
	
	EFI_FILE* kernel = LoadFile(NULL,L"kernel.elf",ImageHandle,SystemTable);
	if(kernel == NULL){
		Print(L"Could not find Kernel\n");
	} else {
		Print(L"Could find kernel\n");
	}
	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		kernel->GetInfo(kernel,&gEfiFileInfoGuid,&FileInfoSize,NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData,FileInfoSize,(void**)&FileInfo);
		kernel->GetInfo(kernel,&gEfiFileInfoGuid,&FileInfoSize,(void**)&FileInfo);

		UINTN size = sizeof(header);
		kernel->Read(kernel,&size,&header);
	}
	if(
		memcmp(&header.e_ident[EI_MAG0], ELFMAG,SELFMAG) == 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 || 
		header.e_ident[EI_DATA] != ELFDATA2LSB || 
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	){
		Print(L"Kernel ELF Header not valid\n");
	} else {
		Print(L"Kernel ELF Header Validated\n");
	}
	Elf64_Phdr * phdrs;
	{
		kernel->SetPosition(kernel,header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData,size,(void**)&phdrs);
		kernel->Read(kernel,&size,phdrs);

	}
	UINTN number_loadable_segments = 1;
	for(
		Elf64_Phdr * phdr = phdrs;
		(char*) phdr < (char*) phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
		){
			switch (phdr->p_type)
			{
			case PT_LOAD:{
				int pages = (phdr->p_memsz + 0x1000-1) /0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress,EfiLoaderData,pages,&segment);
				kernel->SetPosition(kernel,phdr->p_offset);
				UINTN size = phdr->p_filesz;
				kernel->Read(kernel,&size,(void*)segment);
				Print(L"Loaded ELF Segment at %lx \n",segment);
				number_loadable_segments++;
				break;
			    }
			}
		}
	Print(L"Kernel Loaded\n");

	PSF1_FONT* newFont = LoadPSF1Font(NULL, L"zap-light16.psf",ImageHandle,SystemTable);
	if(newFont == NULL){
		Print(L"Font is not valid or not found\n");
	} else {
		Print(L"Font loaded\n");
	}
	FrameBuffer * initializedBuffer = InitGOP();
	Print(L""
	L"Base Address: 0x%lx \n"
	L"BufferSize: %lu \n"
	L"Width: %lu \n"
	L"Height: %lu \n"
	L"PixelPerScanline: %lu \n"
	L"\n", initializedBuffer->BaseAddress,initializedBuffer->BufferSize,
	initializedBuffer->Width,initializedBuffer->Height,initializedBuffer->PixelsPerScanLine);

	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize + number_loadable_segments * DescriptorSize, (void**)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize,Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}
	void (*KernelStart)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*)) header.e_entry);
	BootInfo bootinfo;
	bootinfo.framebuffer = initializedBuffer;
	bootinfo.psf1Font = newFont;
	bootinfo.mMap = Map;
	bootinfo.mMapSize = MapSize;
	bootinfo.mMapDescriptorSize = DescriptorSize;
	
	//print_memorymap(Map,DescriptorSize,MapSize);
	EFI_STATUS exit_stat = SystemTable->BootServices->ExitBootServices(ImageHandle,MapKey);
	if(exit_stat != EFI_SUCCESS){
		asm("mov %0,%%rax \n"
		"hlt"
		: : "r" ((uint64_t)exit_stat << 8) : "rax");
	}
	
	//Print(L"Before altering Memory Map \n");
	UINTN map_ptr_value = (UINTN)Map;
	for(UINTN i = 0; i < MapSize / DescriptorSize; i++, map_ptr_value += DescriptorSize){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)map_ptr_value;
		desc->VirtualStart = (desc->PhysicalStart * 0x1000) / 0x1000;
		if(i < 3){
			desc->VirtualStart += 0x800000000000;
			desc->Type = EfiRuntimeServicesCode;
		}
	}

	UINTN i = ((UINTN)Map) +  MapSize;
	for(
		Elf64_Phdr * phdr = phdrs;
		(char*) phdr < (char*) phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
		){
			switch (phdr->p_type)
			{
			case PT_LOAD:{
				EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)i;
				desc->NumberOfPages = (phdr->p_memsz + 0x1000-1) /0x1000;
				desc->Type = EfiConventionalMemory;
				//desc->Attribute = EFI_MEMORY_WP;
				desc->PhysicalStart = phdr->p_paddr;
				desc->VirtualStart = phdr->p_vaddr;
				//Print(L"Mapped %lx to %lx\n", desc->PhysicalStart,desc->VirtualStart);
				i += DescriptorSize;
				break;
			    }
			}
		}
	EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)(((UINTN)Map) +  MapSize + ((number_loadable_segments-1) * DescriptorSize));
	//desc->Attribute = EFI_MEMORY_UC;
	desc->NumberOfPages = (UINTN)initializedBuffer->BufferSize / 0x1000;
	desc->PhysicalStart = (UINTN)initializedBuffer->BaseAddress;
	desc->VirtualStart = (UINTN)initializedBuffer->BaseAddress;
	desc->Type = EfiBltBufferToVideo;
	print_memorymap(Map,DescriptorSize,MapSize + number_loadable_segments * DescriptorSize);
	//Print(L"Entry Function: %lx \n", KernelStart);
	EFI_STATUS virt_res = SystemTable->RuntimeServices->SetVirtualAddressMap(MapSize,DescriptorSize,DescriptorVersion,Map);
	//Print(L"After set_virtual address map\n");
	if(virt_res != EFI_SUCCESS){
		asm("mov %0,%%rax \n"
			"hlt"
			: : "r" ((uint64_t)virt_res) : "rax");
	} else if (virt_res == EFI_SUCCESS) {
		for(UINTN i = 0; i<initializedBuffer->BufferSize;i++){
			((char*)initializedBuffer->BaseAddress)[i] = 0xff;
		}
		asm("mov $4242424242,%%rax \n"
			"hlt"
			: : : "rax");
	}
	/*

	switch (virt_res)
	{
	case EFI_SUCCESS:
		Print(L"Returned EFI_SUCCESS\n");
		asm("mov $0, %%rax \n"
			"hlt" : : : "rax");
		break;
	case EFI_UNSUPPORTED:
		Print(L"Returned EFI_UNSUPPORTED\n");
		asm("mov $3, %%rax \n"
			"hlt" : : : "rax");
		break;
	default:
		Print(L"Returned Something\n");
		asm("mov $1000, %%rax \n"
			"hlt" : : : "rax");
		break;
	}
	*/
	//while(1){}
	//KernelStart(&bootinfo);
	for(;;) __asm__("hlt");
	return EFI_SUCCESS; // Exit the UEFI application
}

