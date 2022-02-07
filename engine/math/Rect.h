#pragma once

#include <algorithm>
#include "Vector2.h"

namespace engine { namespace math {

template <typename T>
class Rect {
public:
    T x, y, width, height;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(T x, T y, T w, T h) : x(x), y(y), width(w), height(h) {}
    Rect(const Vector2<T>& pos, const Vector2<T>& size)
        : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

    Vector2<T> position() const { return Vector2<T>(x, y); }
    Vector2<T> size() const { return Vector2<T>(width, height); }

    T left()   const { return x; }
    T right()  const { return x + width; }
    T top()    const { return y; }
    T bottom() const { return y + height; }

    Vector2<T> center() const { return Vector2<T>(x + width / 2, y + height / 2); }
    Vector2<T> topLeft() const { return Vector2<T>(x, y); }
    Vector2<T> topRight() const { return Vector2<T>(x + width, y); }
    Vector2<T> bottomLeft() const { return Vector2<T>(x, y + height); }
    Vector2<T> bottomRight() const { return Vector2<T>(x + width, y + height); }

    bool contains(const Vector2<T>& point) const {
        return point.x >= left() && point.x <= right()
            && point.y >= top() && point.y <= bottom();
    }

    bool contains(const Rect& other) const {
        return other.left() >= left() && other.right() <= right()
            && other.top() >= top() && other.bottom() <= bottom();
    }

    bool intersects(const Rect& other) const {
        return left() < other.right() && right() > other.left()
            && top() < other.bottom() && bottom() > other.top();
    }

    Rect intersection(const Rect& other) const {
        if (!intersects(other)) return Rect();
        T l = std::max(left(), other.left());
        T r = std::min(right(), other.right());
        T t = std::max(top(), other.top());
        T b = std::min(bottom(), other.bottom());
        return Rect(l, t, r - l, b - t);
    }

    Rect expanded(T amount) const {
        return Rect(x - amount, y - amount, width + amount * 2, height + amount * 2);
    }

    Rect expanded(T hAmount, T vAmount) const {
        return Rect(x - hAmount, y - vAmount, width + hAmount * 2, height + vAmount * 2);
    }

    static Rect united(const Rect& a, const Rect& b) {
        T l = std::min(a.left(), b.left());
        T r = std::max(a.right(), b.right());
        T t = std::min(a.top(), b.top());
        T btm = std::max(a.bottom(), b.bottom());
        return Rect(l, t, r - l, btm - t);
    }

    Rect translated(T dx, T dy) const {
        return Rect(x + dx, y + dy, width, height);
    }

    bool operator==(const Rect& other) const {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }

    bool operator!=(const Rect& other) const {
        return !(*this == other);
    }
};

using Rectf = Rect<float>;
using Recti = Rect<int>;

}} // namespace engine::math
