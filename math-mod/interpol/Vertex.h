#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <cstddef>
#include "Types.h"

template <class T>
class Vector {
public:
    virtual T& index(size_t i) = 0;
    virtual const T& index(size_t i) const = 0;
    virtual size_t size() = 0;
    virtual void setSize(size_t sz) = 0;

    T& operator[](size_t i) {
        return index(i);
    }
    const T& operator[](size_t i) const {
        return index(i);
    }


    template <class T2>
    Vector operator* (const T2 rh) const {
        Vector<T> rv;
        rv.setSize(this->size());

        for(size_t i=0; i<this->size(); i++) {
            rv[i] *= rh;
        }
        return rv;
    }

    template <class T2>
    const Vector operator*= (const T2 rh)
    {
        Vector<T> rv;
        rv.setSize(this->size());

        for(size_t i=0; i<this->size(); i++) {
            (*this)[i] *= rh;
        }
        return *this;
    }

    template <class T2>
    T operator* (const Vector<T2> & rh) const {
        T rv=0;
        for(size_t i=0; i<rh.size(); i++) {
            rv += (*this)[i] * rh[i];
        }
        return rv;
    }

    Vector& Neg ()
    {
        for(size_t i=0; i<this->size(); i++) {
            (*this)[i] = - (*this)[i];
        }
        return *this;
    }

    Vector operator- () const
    {
        Vector<T> rv;
        rv.setSize(this->size());

        for(size_t i=0; i<this->size(); i++) {
            rv[i] = - (*this)[i];
        }
        return rv;
    }

    template <class T2>
    Vector operator- (const Vector<T2> & rh) const
    {
        Vector<T> rv;
        rv.setSize(this->size());

        for(size_t i=0; i<this->size(); i++) {
            rv[i] = (*this)[i] - rh[i];
        }
        return rv;
    }

    template <class T2>
    const Vector & operator-= (const Vector<T2> & rh)
    {
        for(size_t i=0; i<this->size(); i++) {
            (*this)[i] -= rh[i];
        }

        return *this;
    }

    template <class T2>
    Vector operator+ (const Vector<T2> & rh) const
    {
        Vector<T> rv;
        rv.setSize(this->size());
        for(size_t i=0; i<this->size(); i++) {
            rv[i] = (*this)[i] + rh[i];
        }
        return rv;
    }

    template <class T2>
    const Vector & operator+= (const Vector<T2> & rh)
    {
        for(size_t i=0; i<this->size(); i++) {
            (*this)[i] += rh[i];
        }

        return *this;
    }

    T module () const
    {
        return std::sqrt((*this)*(*this));
    }

    template <class T2>
    const Vector& norm (const T2 mod)
    {
        *this *= mod / this->module();
        return *this;
    }

    template <class T2>
    Vector getNormed (const T2 mod) const
    {
        return (*this) * (mod / this->module());
    }

};

/*
template <class T, unsigned N>
class StaticVector
{
public:
    StaticVector() {
        for(size_t i=0; i<N; i++) {
            vec[i]=0;
        }
    }

    T index() {

    }

    template <class T2>
    Vector operator+ (const Vector<T2, N> & rh) const
    {
        Vector2<T> retval;
        retval.x= x + rh.x;
        retval.y= y + rh.y;
        return retval;
    }
private:
    T vec[];
};
*/

template <class T>
struct Vector2
{
    Vector2(T xx, T yy):x(xx),y(yy)
    {}
    Vector2():x(0),y(0)
    {}

    template <class T2>
    Vector2 operator+ (const Vector2<T2> & rh) const
    {
        Vector2<T> retval;
        retval.x= x + rh.x;
        retval.y= y + rh.y;
        return retval;
    }

    T x,y;
};

typedef Vector2<IntT> Vector2i;
typedef Vector2<FloatT> Vector2d;

typedef Vector2i Point2i;
typedef Vector2i Size2i;

typedef Vector2d Point2d;
typedef Vector2d Size2d;

template <class T>
struct Vector3
{
    template <class T2>
    Vector3(T2 xx, T2 yy, T2 zz):x(xx), y(yy), z(zz)
        {}
    Vector3():x(0), y(0), z(0)
        {}
    template <class T2>
    Vector3(const Vector3<T2>& vec):x(vec.x), y(vec.y), z(vec.z)
        {}

    template <class T2>
    T operator* (const Vector3<T2> & rh) const
        {return x*rh.x + y*rh.y + z*rh.z;}
    template <class T2>
    Vector3 operator* (const T2 rh) const
        {return Vector3<T>(x*rh, y*rh, z*rh);}
    template <class T2>
    const Vector3 operator*= (const T2 rh)
    {
        x*=rh; y*=rh; z*=rh;
        return *this;
    }

    template <class T2>
    Vector3 operator^ (const Vector3<T2>& rh) const
    {
        Vector3<T> retval;
        retval.x= y*rh.z - z*rh.y;
        retval.y= z*rh.x - x*rh.z;
        retval.z= x*rh.y - y*rh.x;
        return retval;
    }
    template <class T2>
    const Vector3 & operator^= (const Vector3<T2> & rh)
    {
        T tx, ty;
        tx = y*rh.z - z*rh.y;
        ty = z*rh.x - x*rh.z;
        z = x*rh.y - y*rh.x;
        x = tx;
        y = ty;
        return *this;
    }
    Vector3& Neg ()
    {
        x= -x;
        y= -y;
        z= -z;
        return *this;
    }
    Vector3 operator- () const
    {
        return Vector3<T>(-x, -y, -z);
    }
    template <class T2>
    Vector3 operator- (const Vector3<T2> & rh) const
    {
        Vector3<T> retval;
        retval.x= x - rh.x;
        retval.y= y - rh.y;
        retval.z= z - rh.z;
        return retval;
    }
    template <class T2>
    const Vector3 & operator-= (const Vector3<T2> & rh)
    {
        x-= rh.x;
        y-= rh.y;
        z-= rh.z;
        return *this;
    }
    template <class T2>
    Vector3 operator+ (const Vector3<T2> & rh) const
    {
        Vector3<T> retval;
        retval.x= x + rh.x;
        retval.y= y + rh.y;
        retval.z= z + rh.z;
        return retval;
    }
    template <class T2>
    const Vector3 & operator+= (const Vector3<T2> & rh)
    {
        x+= rh.x;
        y+= rh.y;
        z+= rh.z;
        return *this;
    }
    T module () const
    {
        return std::sqrt((*this)*(*this));
    }
    template <class T2>
    Vector3& norm (const T2 mod)
    {
        *this *= mod / this->module();
        return *this;
    }
    template <class T2>
    Vector3 getNormed (const T2 mod) const
    {
        return (*this) * (mod / this->module());
    }

    T x,y,z;
};


typedef Vector3<FloatT> Vector3d;
typedef Vector3d Point3d;

struct Ray3d
{
public:
    Point3d pnt;
    Vector3d dir;

    Ray3d(const Point3d& p, const Vector3d & d)
        :pnt(p),dir(d){}
    Ray3d(const Ray3d& r)
        :pnt(r.pnt),dir(r.dir){}

};


#endif // VECTOR_H
