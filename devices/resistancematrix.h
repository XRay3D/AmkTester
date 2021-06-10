#pragma once

#include <QIcon>

struct ResistanceMatrix {
    enum { Size = 11 };
    struct Row {
        int data[Size] {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int& operator[](int i) noexcept { return data[i]; }
        const int& operator[](int i) const noexcept { return data[i]; }
        auto begin() noexcept { return data; }
        auto end() noexcept { return data + Size; }
    } data[Size];

    auto begin() noexcept { return data; }
    auto end() noexcept { return data + Size; }

    Row& operator[](int i) noexcept;
    const Row& operator[](int i) const noexcept;

    friend QDataStream& operator<<(QDataStream& stream, const ResistanceMatrix& pins) {
        for(int x = 0; x < Size; ++x)
            for(int y = 0; y < Size; ++y)
                stream << pins.data[y][x];
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, ResistanceMatrix& pins) {
        for(int x = 0; x < Size; ++x)
            for(int y = 0; y < Size; ++y)
                stream >> pins.data[y][x];
        return stream;
    }

    void reset() noexcept;

    QIcon toIcon(QSize size) const noexcept;

    bool operator==(const ResistanceMatrix& other) const noexcept;
};
