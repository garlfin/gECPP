//
// Created by scion on 4/16/2025.
//

#pragma once

#include <set>
#include <Core/Math/Math.h>

#define SPLINE_LENGTH_ITERATIONS 16

enum class DistanceMode : u8
{
    Relative,
    Absolute
};

template<Dimension DIMENSION>
struct SplinePoint
{
    ::Position<DIMENSION> Position = DEFAULT;
    ::Position<DIMENSION> Tangent = DEFAULT;

    SplinePoint operator+(const SplinePoint& b) const;
    SplinePoint operator-(const SplinePoint& b) const;
    SplinePoint operator*(float b) const;
};

template<Dimension DIMENSION>
SplinePoint<DIMENSION> operator*(float a, const SplinePoint<DIMENSION>& b) { return b * a; }

template<class T>
struct SplineDataPoint
{
    mutable T Value = DEFAULT;
    float Distance = DEFAULT;
};

template<class T>
struct SplineDataPointCompare
{
    using Point = SplineDataPoint<T>;

    bool operator()(const Point& a, const Point& b) { return a.Distance < b.Distance; }
    bool operator()(const Point& a, float b) { return a.Distance < b; }
    bool operator()(float a, const Point& b) { return a < b.Distance; }
};

template<class T>
struct CubicInterpolation
{
    constexpr T Interpolate(float t, const T& a, const T& b, const T& c) const;
    constexpr T Interpolate(float t, const T& a, const T& b) const;
};

template<class POINT_T, class INTERPOLATOR_T>
concept interpolator = requires(const POINT_T& point, const INTERPOLATOR_T& interpolator)
{
    { interpolator.Interpolate(0.f, point, point, point) } -> std::same_as<POINT_T>;
    { interpolator.Interpolate(0.f, point, point) } -> std::same_as<POINT_T>;
};

template<Dimension DIMENSION = Dimension::D3D>
class Spline
{
public:
    using Point = SplinePoint<DIMENSION>;
    using Position = ::Position<DIMENSION>;

    float GetLength() const;
    void AddPoint(const Point& point, size_t index = -1);
    void SetPoint(size_t index, const Point&);
    const Point& GetPoint(size_t index);

    template<class INTERPOLATOR = CubicInterpolation<Point>> requires interpolator<Point, INTERPOLATOR>
    Point Evaluate(DistanceMode mode, float distance, INTERPOLATOR interpolator = DEFAULT) const;

private:
    std::vector<Point> _points;

    mutable float _length = DEFAULT;
    mutable bool _lengthInvalidated = true;
};

template<class T, Dimension DIMENSION = Dimension::D3D>
class SplineData
{
public:
    using Point = SplineDataPoint<T>;
    using InterpolationFunc = T(*)(const Point* a, const Point* b, const Point* c, float t);

    SplineData(Spline<DIMENSION>& spline, DistanceMode mode);

    GET_CONST(Spline<DIMENSION>&, Spline, *_spline);
    GET_CONST(DistanceMode, Mode, _mode);

    void SetMode(DistanceMode mode, bool adjustPoints = true);
    void SetSpline(Spline<DIMENSION>& spline, bool adjustPoints = true);
    void AddPoint(const T& point);

    const Point& GetPoint(DistanceMode mode, float distance);
    const Point& GetPoint(size_t index);

    void SetPoint(DistanceMode mode, float distance, const Point& point);
    void SetPoint(size_t index, const Point& point);

    template<class INTERPOLATOR = CubicInterpolation<T>> requires interpolator<Point, INTERPOLATOR>
    T Evaluate(DistanceMode mode, float distance, INTERPOLATOR interpolator = DEFAULT) const;

private:
    float AdjustDistance(DistanceMode mode, float distance) const;

    std::set<T, SplineDataPointCompare<T>> _points;
    Spline<DIMENSION>* _spline = nullptr;
    DistanceMode _mode = DistanceMode::Relative;
};

#include "Spline.inl"