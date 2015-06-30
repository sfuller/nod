#include <stdio.h>
#include <stdexcept>
#include "IDiscIO.hpp"

namespace NOD
{

class DiscIOWBFS : public IDiscIO
{
    std::string filepath;
public:
    DiscIOWBFS(const std::string& fpin)
    : filepath(fpin) {}

    class ReadStream : public IReadStream
    {
        friend class DiscIOWBFS;
        FILE* fp;
        ReadStream(FILE* fpin)
        : fp(fpin) {}
        ~ReadStream() {fclose(fp);}
    public:
        uint64_t read(void* buf, uint64_t length)
        {return fread(buf, 1, length, fp);}
        void seek(int64_t offset, int whence)
        {fseeko(fp, offset, whence);}
    };
    std::unique_ptr<IReadStream> beginReadStream(uint64_t offset) const
    {
        FILE* fp = fopen(filepath.c_str(), "rb");
        if (!fp)
        {
            throw std::runtime_error("Unable to open '" + filepath + "' for reading");
            return std::unique_ptr<IReadStream>();
        }
        fseeko(fp, offset, SEEK_SET);
        return std::unique_ptr<IReadStream>(new ReadStream(fp));
    }

    class WriteStream : public IWriteStream
    {
        friend class DiscIOWBFS;
        FILE* fp;
        WriteStream(FILE* fpin)
        : fp(fpin) {}
        ~WriteStream() {fclose(fp);}
    public:
        uint64_t write(void* buf, uint64_t length)
        {return fwrite(buf, 1, length, fp);}
    };
    std::unique_ptr<IWriteStream> beginWriteStream(uint64_t offset) const
    {
        FILE* fp = fopen(filepath.c_str(), "wb");
        if (!fp)
        {
            throw std::runtime_error("Unable to open '" + filepath + "' for writing");
            return std::unique_ptr<IWriteStream>();
        }
        fseeko(fp, offset, SEEK_SET);
        return std::unique_ptr<IWriteStream>(new WriteStream(fp));
    }
};

std::unique_ptr<IDiscIO> NewDiscIOWBFS(const char* path)
{
    return std::unique_ptr<IDiscIO>(new DiscIOWBFS(path));
}

}

