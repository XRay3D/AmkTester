#pragma once

#include <QIcon>

struct Pins {
    enum { Count = 11 };
    struct Row {
        int data[Count]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int& operator[](int i) noexcept { return data[i]; }
        const int& operator[](int i) const noexcept { return data[i]; }
    } data[Count];
    Row& operator[](int i) noexcept;
    const Row& operator[](int i) const noexcept;

    friend QDataStream& operator<<(QDataStream& stream, const Pins& pins) {
        for(int x = 0; x < Count; ++x)
            for(int y = 0; y < Count; ++y)
                stream << pins.data[y][x];
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, Pins& pins) {
        for(int x = 0; x < Count; ++x)
            for(int y = 0; y < Count; ++y)
                stream >> pins.data[y][x];
        return stream;
    }

    void reset() noexcept;

    QIcon toIcon(QSize size) const noexcept;

    bool operator==(const Pins& other) const noexcept;
};
