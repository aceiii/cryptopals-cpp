#ifndef __BLOCKS_HPP__
#define __BLOCKS_HPP__

template <typename T>
std::vector<T> split_blocks(const T& buf, const size_t& block_size) {
    const int num_blocks = buf.size() / block_size;
    const int remainder = buf.size() % block_size;

    std::vector<T> blocks;
    for (int i = 0; i < num_blocks; i += 1) {
        auto start = next(begin(buf), i * block_size);
        auto stop = next(start, block_size);
        T block(start, stop);
        blocks.push_back(block);
    }
    if (remainder > 0) {
        auto start = next(begin(buf), num_blocks * block_size);
        auto stop = end(buf);
        T block(start, stop);
        blocks.push_back(block);
    }

    return blocks;
}

byte_vector combine_blocks(const std::vector<byte_vector>& blocks) {
    byte_vector buffer;

    for (auto it = begin(blocks); it != end(blocks); it++) {
        const byte_vector& block = *it;
        std::copy(begin(block), end(block), std::back_inserter(buffer));
    }

    return buffer;
}

#endif//__BLOCKS_HPP__
