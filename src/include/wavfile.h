#ifndef WAVFILE_H
#define WAVFILE_H

#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

// NOTE: we use standard PCM RIFF WAV header with 44 bytes size.
struct WavHeader {

    char chunk_id[4];

    uint32_t chunk_size;

    char format[4];

    char subchunk_id[4];

    uint32_t subchunk_size;

    uint16_t audio_format;

    uint16_t num_channels;

    uint32_t sample_rate;

    uint32_t byte_rate;

    uint16_t block_align;

    uint16_t bits_per_sample;

    char data_id[4];

    uint32_t data_size;

};

class WavFileReader {
public:
    explicit WavFileReader(const std::string &path, size_t bsf) :
            path(path), header({}), buffer_size_factor(bsf) {
        ifs.open(path, std::ios::binary);
        if (!ifs.is_open()) {
            std::stringstream ss;
            ss << "Error opening file " << path << " for reading.";
            throw std::runtime_error(ss.str());
        }
        ifs.read((char*) (&header), sizeof(WavHeader));
    }

    ~WavFileReader() { ifs.close(); }

    std::streamsize read(std::vector<int16_t> &container) {
        container.reserve(buffer_size_factor * header.num_channels);
        ifs.read((char*) container.data(),
                            sizeof(int16_t)
                            * buffer_size_factor
                            * header.num_channels);
        auto lokek = std::streamoff(ifs.tellg());
        auto res = ifs.gcount();
        container.resize(res > 0? uint64_t(res) / sizeof(int16_t): 0);
        return res;
    }

    const std::string &getPath() const { return path; }

    const WavHeader &getHeader() const { return header; }

private:
    std::string path;
    std::ifstream ifs;
    WavHeader header;
    size_t buffer_size_factor;
};

class WavFileWriter {
public:
    explicit WavFileWriter(const std::string &path) :
            path(path), header({}) {
        ofs.open(path, std::ios::binary);
        if (!ofs.is_open()) {
            std::stringstream ss;
            ss << "Error opening file " << path << " for writing.";
            throw std::runtime_error(ss.str());
        }
    }

    ~WavFileWriter() { ofs.close(); }

    bool writeHeader() {
        auto curpos = std::streamoff(ofs.tellp());
        ofs.seekp(0, std::ios::beg);
        auto res = bool(ofs.write((char*) (&header), sizeof(WavHeader)));
        ofs.seekp(curpos > 0 ? curpos : sizeof(WavHeader), std::ios::beg);
        return res;
    }

    bool write(const std::vector<int16_t> &data) {
        auto res = bool(ofs.write((char*) (data.data()), sizeof(int16_t) * data.size()));
        return res;
    }

    const std::string &getPath() const { return path; }

    const WavHeader &getHeader() const { return header; }

    WavHeader &getHeader() { return header; }

private:
    std::string path;
    std::ofstream ofs;
    WavHeader header;
};

#endif
