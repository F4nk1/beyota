/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  ring_buffer.h                                                         */
/**************************************************************************/

#pragma once

#include "core/templates/local_vector.h"
#include "core/type_primitives.h"

#include <algorithm>
#include <cassert>

namespace Beyota {

template <typename T>
class RingBuffer {
    LocalVector<T> data_;
    i32 read_pos_{0};
    i32 write_pos_{0};
    i32 size_mask_{0};

    i32 inc(i32 &p_var, i32 p_size) const noexcept {
        i32 ret = p_var;
        p_var = (p_var + p_size) & size_mask_;
        return ret;
    }

public:
    explicit RingBuffer(i32 p_power = 0) {
        resize(p_power);
    }

    [[nodiscard]] i32 size() const noexcept {
        return (i32)data_.size();
    }

    [[nodiscard]] i32 space_left() const noexcept {
        i32 left = read_pos_ - write_pos_;
        if (left < 0) {
            return size() + left - 1;
        }
        if (left == 0) {
            return size() - 1;
        }
        return left - 1;
    }

    [[nodiscard]] i32 data_left() const noexcept {
        return size() - space_left() - 1;
    }

    void clear() noexcept {
        read_pos_ = 0;
        write_pos_ = 0;
    }

    void resize(i32 p_power) {
        if (p_power <= 0) {
            data_.clear();
            read_pos_ = 0;
            write_pos_ = 0;
            size_mask_ = 0;
            return;
        }

        i32 old_size = size();
        i32 new_size = 1 << p_power;
        i32 mask = new_size - 1;
        data_.resize((u32)new_size);

        if (old_size < new_size && read_pos_ > write_pos_) {
            for (i32 i = 0; i < write_pos_; ++i) {
                data_[(old_size + i) & mask] = std::move(data_[i]);
            }
            write_pos_ = (old_size + write_pos_) & mask;
        } else {
            read_pos_ = read_pos_ & mask;
            write_pos_ = write_pos_ & mask;
        }

        size_mask_ = mask;
    }

    bool write(const T &p_v) {
        if (space_left() < 1) {
            return false;
        }
        data_[inc(write_pos_, 1)] = p_v;
        return true;
    }

    i32 write(const T *p_buf, i32 p_size) {
        if (p_buf == nullptr || p_size <= 0) {
            return 0;
        }
        i32 left = space_left();
        p_size = std::min(left, p_size);

        i32 pos = write_pos_;
        i32 to_write = p_size;
        i32 src = 0;

        while (to_write > 0) {
            i32 end = std::min(pos + to_write, size());
            i32 total = end - pos;

            for (i32 i = 0; i < total; ++i) {
                data_[pos + i] = p_buf[src++];
            }
            to_write -= total;
            pos = 0;
        }

        inc(write_pos_, p_size);
        return p_size;
    }

    [[nodiscard]] T read() {
        assert(data_left() >= 1);
        return data_[inc(read_pos_, 1)];
    }

    i32 read(T *p_buf, i32 p_size, bool p_advance = true) {
        if (p_buf == nullptr || p_size <= 0) {
            return 0;
        }
        i32 left = data_left();
        p_size = std::min(left, p_size);

        i32 pos = read_pos_;
        i32 to_read = p_size;
        i32 dst = 0;

        while (to_read > 0) {
            i32 end = std::min(pos + to_read, size());
            i32 total = end - pos;

            for (i32 i = 0; i < total; ++i) {
                p_buf[dst++] = data_[pos + i];
            }
            to_read -= total;
            pos = 0;
        }

        if (p_advance) {
            inc(read_pos_, p_size);
        }
        return p_size;
    }

    [[nodiscard]] i32 copy(T *p_buf, i32 p_offset, i32 p_size) const {
        if (p_buf == nullptr || p_size <= 0) {
            return 0;
        }
        i32 left = data_left();
        if (p_offset + p_size > left) {
            p_size = left - p_offset;
            if (p_size <= 0) {
                return 0;
            }
        }
        p_size = std::min(left, p_size);

        i32 pos = (read_pos_ + p_offset) & size_mask_;
        i32 to_read = p_size;
        i32 dst = 0;

        while (to_read > 0) {
            i32 end = std::min(pos + to_read, size());
            i32 total = end - pos;

            for (i32 i = 0; i < total; ++i) {
                p_buf[dst++] = data_[pos + i];
            }
            to_read -= total;
            pos = 0;
        }
        return p_size;
    }

    [[nodiscard]] i32 find(const T &p_val, i32 p_offset, i32 p_max_size) const {
        i32 left = data_left();
        if (p_offset + p_max_size > left) {
            p_max_size = left - p_offset;
            if (p_max_size <= 0) {
                return -1;
            }
        }
        p_max_size = std::min(left, p_max_size);

        i32 pos = (read_pos_ + p_offset) & size_mask_;
        i32 to_read = p_max_size;

        while (to_read > 0) {
            i32 end = std::min(pos + to_read, size());
            i32 total = end - pos;

            for (i32 i = 0; i < total; ++i) {
                if (data_[pos + i] == p_val) {
                    return i + (p_max_size - to_read);
                }
            }
            to_read -= total;
            pos = 0;
        }
        return -1;
    }

    i32 advance_read(i32 p_n) noexcept {
        p_n = std::min(p_n, data_left());
        inc(read_pos_, p_n);
        return p_n;
    }

    i32 decrease_write(i32 p_n) noexcept {
        p_n = std::min(p_n, data_left());
        inc(write_pos_, size_mask_ + 1 - p_n);
        return p_n;
    }
};

} // namespace Beyota
