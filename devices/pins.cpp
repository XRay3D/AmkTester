#include "pins.h"

Pins::Row& Pins::operator[](int i) noexcept { return data[i]; }

const Pins::Row& Pins::operator[](int i) const noexcept { return data[i]; }

void Pins::reset() noexcept {
    for(auto& row : data)
        row = {};
}

QIcon Pins::toIcon(QSize size) const noexcept {
    QImage image{QSize{Count, Count}, QImage::Format_ARGB32};
    for(int x = 0; x < Count; ++x) {
        for(int y = 0; y < Count; ++y) {
            image.setPixel(x, y, data[y][x] == -1 ? 0x40000000 : 0xFF000000);
        }
    }

    return QPixmap::fromImage(image.scaled(size.width(), size.height()));
}

bool Pins::operator==(const Pins& other) const noexcept {
    for(int x = 0; x < Count; ++x) {
        for(int y = 0; y < Count; ++y) {
            //qDebug() << __FUNCTION__ << x << y << data[y][x] << other.data[y][x];
            if(data[y][x] > -1 && other.data[y][x] > -1) {
                if(abs(data[y][x] - other.data[y][x]) > 10)
                    return false;
            } else if(data[y][x] != other.data[y][x])
                return false;
        }
    }
    return true;
}
