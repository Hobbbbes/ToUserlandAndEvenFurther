#pragma once
#include <stdint.h>
namespace FS {

    class Abstract_File {
        public:
            virtual uint64_t read(void* buff,uint64_t posInFile, uint64_t count) = 0;
            virtual uint64_t write(void* buff,uint64_t posInFile, uint64_t count) = 0;
        protected:
            uint64_t size;
    };

    enum class Seek : uint8_t {
        Offset,
        Start,
        Current
    };

    class FileHandel { //Shared Pointer to File with own view of file
        public:
            inline uint64_t read(void* buff,uint64_t count){
                uint64_t bytesRead = file->read(buff,offset,count);
                offset += bytesRead;
                return bytesRead;
            }
            inline uint64_t write(void* buff, uint64_t count){
                uint64_t bytesWritten = file->write(buff,offset,count);
                offset += bytesWritten;
                return bytesWritten;
            }

            void seek(uint64_t count, Seek mode);

            FileHandel(Abstract_File* file);
            FileHandel(const FileHandel& handel);
            FileHandel(FileHandel&& handel);
            FileHandel& operator=(const FileHandel& handel);
            FileHandel& operator=(const FileHandel&& handel);

        private:
            uint64_t* refCount;
            Abstract_File* file;
            uint64_t offset;

        friend Abstract_File;
    };
}