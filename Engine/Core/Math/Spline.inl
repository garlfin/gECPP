//
// Created by scion on 4/16/2025.
//

#pragma once

#include "Spline.h"

template <class T>
constexpr T CubicInterpolation<T>::Interpolate(float t, const T& a, const T& b, const T& c) const
{
    const T at = a * t, bt = b * t, ct = c * t;
    return a - 2.f * at + 2.f * bt + at * t - 2.f * bt * t + ct * t;
}

template <class T>
constexpr T CubicInterpolation<T>::Interpolate(float t, const T& a, const T& b) const
{
    return t * b + (1.f - t) * a;
}

template <Dimension DIMENSION>
float Spline<DIMENSION>::GetLength() const
{
    const size_t samples = GetSegmentCount() * SPLINE_LENGTH_ITERATIONS;

    if(!_lengthInvalidated)
        return _length;

    _lengthInvalidated = false;

    if(_points.size() <= 1)
        return _length = 0.f;

    if(_points.size() == 2)
        return _length = glm::distance(_points[0].Position, _points[1].Position) * 2.f;

    Position previousPosition = Evaluate(DistanceMode::Relative, 0.f).Position;
    for(size_t i = 0; i < samples; i++)
    {
        const Position newPosition = Evaluate(DistanceMode::Relative, (float)(i + 1) / samples).Position;

        _length += glm::distance(previousPosition, newPosition);
        previousPosition = newPosition;
    }

    return _length;
}

template <Dimension DIMENSION>
void Spline<DIMENSION>::AddPoint(const Point& point, size_t index)
{
    if(index == -1u) index = _points.size();
    _points.insert(_points.begin() + std::min(index, _points.size()), point);
    _lengthInvalidated = true;
}

template <Dimension DIMENSION>
void Spline<DIMENSION>::SetPoint(size_t index, const Point& point)
{
    GE_ASSERT(index < _points.size());
    _lengthInvalidated = true;
    _points[index] = point;
}

template <Dimension DIMENSION>
const typename Spline<DIMENSION>::Point& Spline<DIMENSION>::GetPoint(size_t index)
{
    GE_ASSERT(index < _points.size());
    return _points[index];
}

template <Dimension DIMENSION>
template <class INTERPOLATOR> requires interpolator<SplinePoint<DIMENSION>, INTERPOLATOR>
typename Spline<DIMENSION>::Point Spline<DIMENSION>::Evaluate(DistanceMode mode, float distance, INTERPOLATOR interpolator) const
{
    distance = AdjustDistance(mode, distance);
    if(mode == DistanceMode::Absolute)
        distance /= GetLength();

    if(_points.empty()) return DEFAULT;
    if(!GetSegmentCount() || GetSegmentCount() % 2 == 1) return DEFAULT;

    const float delta = GetSegmentCount() / 2.f;
    float fac = distance * delta;
    size_t basePoint = fac;
    fac -= basePoint;
    basePoint *= 2;

    if(_points.size() - basePoint == 0)
            return _points.back();

    const SplinePoint<DIMENSION>& a = _points[basePoint];
    const SplinePoint<DIMENSION>& b = _points[basePoint + 1];
    const SplinePoint<DIMENSION>& c = _points[basePoint + 2];

    return interpolator.Interpolate(fac, a, b, c);
}

template <Dimension DIMENSION>
float Spline<DIMENSION>::AdjustDistance(DistanceMode mode, float distance) const
{
    const float denominator = (mode == DistanceMode::Absolute ? GetLength() : 1.f) + FLT_EPSILON;

    if(distance >= 0)
        distance = glm::mod(distance, denominator);
    else
        distance = denominator - glm::mod(distance, denominator);

    return distance;
}

template <Dimension DIMENSION>
SplinePoint<DIMENSION> SplinePoint<DIMENSION>::operator+(const SplinePoint& b) const
{
    return SplinePoint{ Position + b.Position, Tangent + b.Tangent };
}

template <Dimension DIMENSION>
SplinePoint<DIMENSION> SplinePoint<DIMENSION>::operator-(const SplinePoint& b) const
{
    return SplinePoint{ Position - b.Position, Tangent - b.Tangent };
}

template <Dimension DIMENSION>
SplinePoint<DIMENSION> SplinePoint<DIMENSION>::operator*(float b) const
{
    return SplinePoint{ Position * b, Tangent * b };
}

template <class T, Dimension DIMENSION>
SplineData<T, DIMENSION>::SplineData(Spline<DIMENSION>& spline, DistanceMode mode) :
    _spline(&spline),
    _mode(mode)
{
}

template <class T, Dimension DIMENSION>
void SplineData<T, DIMENSION>::SetMode(DistanceMode mode, bool adjustPoints)
{
    if(_mode == mode) return;
    _mode = mode;

    if(!adjustPoints) return;

    for(Point& point : _points)
        if(mode == DistanceMode::Relative) // from Absolute to Relative
            point.Distance /= _spline.GetLength();
        else // Relative to Absolute
            point.Distance *= _spline->GetLength();
}

template <class T, Dimension DIMENSION>
void SplineData<T, DIMENSION>::SetSpline(Spline<DIMENSION>& spline, bool adjustPoints)
{
    if(_spline == &spline) return;

    Spline<DIMENSION>& oldSpline = _spline;
    _spline = &spline;

    if(!adjustPoints || _mode == DistanceMode::Relative || oldSpline.GetLength() == _spline->GetLength())
        return;

    for(Point& point : _points)
        point.Distance = point.Distance / oldSpline.GetLength() * _spline->GetLength();
}

template <class T, Dimension DIMENSION>
void SplineData<T, DIMENSION>::AddPoint(const T& point)
{
    _points.insert(point);
}

template <class T, Dimension DIMENSION>
const typename SplineData<T, DIMENSION>::Point& SplineData<T, DIMENSION>::GetPoint(DistanceMode mode, float distance)
{
    return *std::lower_bound(_points, AdjustDistance(mode, distance));
}

template <class T, Dimension DIMENSION>
const typename SplineData<T, DIMENSION>::Point& SplineData<T, DIMENSION>::GetPoint(size_t index)
{
    GE_ASSERT(index < _points.size());

    auto begin = _points.cbegin();
    for(size_t i = 0; i < index; i++) ++begin; // cannot add to set iterator
    return *begin;
}

template <class T, Dimension DIMENSION>
void SplineData<T, DIMENSION>::SetPoint(DistanceMode mode, float distance, const Point& point)
{
    *std::lower_bound(_points, AdjustDistance(mode, distance)) = point;
}

template <class T, Dimension DIMENSION>
void SplineData<T, DIMENSION>::SetPoint(size_t index, const Point& point)
{
    GE_ASSERT(index < _points.size());

    auto begin = _points.cbegin();
    for(size_t i = 0; i < index; i++) ++begin; // cannot add to set iterator
    *begin = point;
}

template <class T, Dimension DIMENSION>
template <class INTERPOLATOR> requires interpolator<SplineDataPoint<T>, INTERPOLATOR>
T SplineData<T, DIMENSION>::Evaluate(DistanceMode mode, float distance, INTERPOLATOR interpolator) const
{
    return DEFAULT;
}

template <class T, Dimension DIMENSION>
float SplineData<T, DIMENSION>::AdjustDistance(DistanceMode mode, float distance) const
{
    const float denominator = mode == DistanceMode::Absolute ? _spline->GetLength() : 1.f;

    if(distance >= 0)
        distance = glm::mod(distance, denominator);
    else
        distance = denominator - glm::mod(distance, denominator);

    if(mode == _mode) return distance;

    if(mode == DistanceMode::Relative) // to relative
        distance /= _spline->GetLength();
    else // to absolute
        distance *= _spline->GetLength();

    return distance;
}
