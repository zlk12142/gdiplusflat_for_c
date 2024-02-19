/**************************************************************************\
*
* Copyright (c) 1998-2001, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusTypes.h
*
* Abstract:
*
*   GDI+ Types
*
\**************************************************************************/

#ifndef _GDIPLUSTYPES_H
#define _GDIPLUSTYPES_H

//--------------------------------------------------------------------------
// Callback functions
//--------------------------------------------------------------------------

typedef BOOL (CALLBACK * ImageAbort)(VOID *);
typedef ImageAbort DrawImageAbort;
typedef ImageAbort GetThumbnailImageAbort;

// Callback for EnumerateMetafile methods.  The parameters are:

//      recordType      WMF, EMF, or EMF+ record type
//      flags           (always 0 for WMF/EMF records)
//      dataSize        size of the record data (in bytes), or 0 if no data
//      data            pointer to the record data, or NULL if no data
//      callbackData    pointer to callbackData, if any

// This method can then call Metafile::PlayRecord to play the
// record that was just enumerated.  If this method  returns
// FALSE, the enumeration process is aborted.  Otherwise, it continues.

typedef BOOL (CALLBACK * EnumerateMetafileProc)(EmfPlusRecordType,UINT,UINT,const BYTE*,VOID*);

#if (GDIPVER >= 0x0110)
// This is the main GDI+ Abort interface

typedef struct _GdiplusAbort
{
    HRESULT (__stdcall *Abort)(void);
} GdiplusAbort;
#endif //(GDIPVER >= 0x0110)

//--------------------------------------------------------------------------
// Primitive data types
//
// NOTE:
//  Types already defined in standard header files:
//      INT8
//      UINT8
//      INT16
//      UINT16
//      INT32
//      UINT32
//      INT64
//      UINT64
//
//  Avoid using the following types:
//      LONG - use INT
//      ULONG - use UINT
//      DWORD - use UINT32
//--------------------------------------------------------------------------

typedef float REAL;

#define REAL_MAX            FLT_MAX
#define REAL_MIN            FLT_MIN
#define REAL_TOLERANCE     (FLT_MIN * 100)
#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */

//--------------------------------------------------------------------------
// Forward declarations of common classes
//--------------------------------------------------------------------------

struct _Size;
struct _SizeF;
struct _Point;
struct _PointF;
struct _Rect;
struct _RectF;
struct _CharacterRange;

//--------------------------------------------------------------------------
// Status return values from GDI+ methods
//--------------------------------------------------------------------------

typedef enum _Status
{
    Ok = 0,
    GenericError = 1,
    InvalidParameter = 2,
    OutOfMemory = 3,
    ObjectBusy = 4,
    InsufficientBuffer = 5,
    NotImplemented = 6,
    Win32Error = 7,
    WrongState = 8,
    Aborted = 9,
    FileNotFound = 10,
    ValueOverflow = 11,
    AccessDenied = 12,
    UnknownImageFormat = 13,
    FontFamilyNotFound = 14,
    FontStyleNotFound = 15,
    NotTrueTypeFont = 16,
    UnsupportedGdiplusVersion = 17,
    GdiplusNotInitialized = 18,
    PropertyNotFound = 19,
    PropertyNotSupported = 20,
#if (GDIPVER >= 0x0110)
    ProfileNotFound = 21,
#endif //(GDIPVER >= 0x0110)
} Status;

//--------------------------------------------------------------------------
// Represents a dimension in a 2D coordinate system (floating-point coordinates)
//--------------------------------------------------------------------------

typedef struct _SizeF
{
    REAL Width;
    REAL Height;
} SizeF;
//(1) SizeF()
//(2) {
//(2)     Width = Height = 0.0f;
//(0) }

//(1) SizeF(IN const SizeF& size)
//(2) {
//(2)     Width = size.Width;
//(2)     Height = size.Height;
//(0) }

//(-1) SizeF(IN REAL width,
//(1)       IN REAL height)
//(2) {
//(2)     Width = width;
//(2)     Height = height;
//(0) }

//(1) SizeF operator+(IN const SizeF& sz) const
//(2) {
//(2)     return SizeF(Width + sz.Width,
//(2)                  Height + sz.Height);
//(0) }

//(1) SizeF operator-(IN const SizeF& sz) const
//(2) {
//(2)     return SizeF(Width - sz.Width,
//(2)                  Height - sz.Height);
//(0) }

//(1) BOOL Equals(IN const SizeF& sz) const
//(2) {
//(2)     return (Width == sz.Width) && (Height == sz.Height);
//(0) }

//(1) BOOL Empty() const
//(2) {
//(2)     return (Width == 0.0f && Height == 0.0f);
//(0) }



//--------------------------------------------------------------------------
// Represents a dimension in a 2D coordinate system (integer coordinates)
//--------------------------------------------------------------------------

typedef struct _Size
{
    INT Width;
    INT Height;
} Size;
//(1) Size()
//(2) {
//(2)     Width = Height = 0;
//(0) }

//(1) Size(IN const Size& size)
//(2) {
//(2)     Width = size.Width;
//(2)     Height = size.Height;
//(0) }

//(-1) Size(IN INT width,
//(1)      IN INT height)
//(2) {
//(2)     Width = width;
//(2)     Height = height;
//(0) }

//(1) Size operator+(IN const Size& sz) const
//(2) {
//(2)     return Size(Width + sz.Width,
//(2)                 Height + sz.Height);
//(0) }

//(1) Size operator-(IN const Size& sz) const
//(2) {
//(2)     return Size(Width - sz.Width,
//(2)                 Height - sz.Height);
//(0) }

//(1) BOOL Equals(IN const Size& sz) const
//(2) {
//(2)     return (Width == sz.Width) && (Height == sz.Height);
//(0) }

//(1) BOOL Empty() const
//(2) {
//(2)     return (Width == 0 && Height == 0);
//(0) }



//--------------------------------------------------------------------------
// Represents a location in a 2D coordinate system (floating-point coordinates)
//--------------------------------------------------------------------------

typedef struct _PointF
{
    REAL X;
    REAL Y;
} PointF;
//(1) PointF()
//(2) {
//(2)    X = Y = 0.0f;
//(0) }

//(1) PointF(IN const PointF &point)
//(2) {
//(2)    X = point.X;
//(2)    Y = point.Y;
//(0) }

//(1) PointF(IN const SizeF &size)
//(2) {
//(2)    X = size.Width;
//(2)    Y = size.Height;
//(0) }

//(-1) PointF(IN REAL x,
//(1)       IN REAL y)
//(2) {
//(2)    X = x;
//(2)    Y = y;
//(0) }

//(1) PointF operator+(IN const PointF& point) const
//(2) {
//(2)    return PointF(X + point.X,
//(2)                  Y + point.Y);
//(0) }

//(1) PointF operator-(IN const PointF& point) const
//(2) {
//(2)    return PointF(X - point.X,
//(2)                  Y - point.Y);
//(0) }

//(1) BOOL Equals(IN const PointF& point)
//(2) {
//(2)    return (X == point.X) && (Y == point.Y);
//(0) }



//--------------------------------------------------------------------------
// Represents a location in a 2D coordinate system (integer coordinates)
//--------------------------------------------------------------------------

typedef struct _Point
{
    INT X;
    INT Y;
} Point;
//(1) Point()
//(2) {
//(2)    X = Y = 0;
//(0) }

//(1) Point(IN const Point &point)
//(2) {
//(2)    X = point.X;
//(2)    Y = point.Y;
//(0) }

//(1) Point(IN const Size &size)
//(2) {
//(2)    X = size.Width;
//(2)    Y = size.Height;
//(0) }

//(-1) Point(IN INT x,
//(1)      IN INT y)
//(2) {
//(2)    X = x;
//(2)    Y = y;
//(0) }

//(1) Point operator+(IN const Point& point) const
//(2) {
//(2)    return Point(X + point.X,
//(2)                 Y + point.Y);
//(0) }

//(1) Point operator-(IN const Point& point) const
//(2) {
//(2)    return Point(X - point.X,
//(2)                 Y - point.Y);
//(0) }

//(1) BOOL Equals(IN const Point& point)
//(2) {
//(2)    return (X == point.X) && (Y == point.Y);
//(0) }



//--------------------------------------------------------------------------
// Represents a rectangle in a 2D coordinate system (floating-point coordinates)
//--------------------------------------------------------------------------

typedef struct _RectF
{
    REAL X;
    REAL Y;
    REAL Width;
    REAL Height;
} RectF;

//(1) RectF()
//(2) {
//(2)     X = Y = Width = Height = 0.0f;
//(0) }

//(-1) RectF(IN REAL x,
//(-1)       IN REAL y,
//(-1)       IN REAL width,
//(1)       IN REAL height)
//(2) {
//(2)     X = x;
//(2)     Y = y;
//(2)     Width = width;
//(2)     Height = height;
//(0) }

//(-1) RectF(IN const PointF& location,
//(1)       IN const SizeF& size)
//(2) {
//(2)     X = location.X;
//(2)     Y = location.Y;
//(2)     Width = size.Width;
//(2)     Height = size.Height;
//(0) }

//(1) RectF* Clone() const
//(2) {
//(2)     return new RectF(X, Y, Width, Height);
//(0) }

//(1) VOID GetLocation(OUT PointF* point) const
//(2) {
//(2)     point->X = X;
//(2)     point->Y = Y;
//(0) }

//(1) VOID GetSize(OUT SizeF* size) const
//(2) {
//(2)     size->Width = Width;
//(2)     size->Height = Height;
//(0) }

//(1) VOID GetBounds(OUT RectF* rect) const
//(2) {
//(2)     rect->X = X;
//(2)     rect->Y = Y;
//(2)     rect->Width = Width;
//(2)     rect->Height = Height;
//(0) }

//(1) REAL GetLeft() const
//(2) {
//(2)     return X;
//(0) }

//(1) REAL GetTop() const
//(2) {
//(2)     return Y;
//(0) }

//(1) REAL GetRight() const
//(2) {
//(2)     return X+Width;
//(0) }

//(1) REAL GetBottom() const
//(2) {
//(2)     return Y+Height;
//(0) }

//(1) BOOL IsEmptyArea() const
//(2) {
//(2)     return (Width <= REAL_EPSILON) || (Height <= REAL_EPSILON);
//(0) }

//(1) BOOL Equals(IN const RectF & rect) const
//(2) {
//(2)     return X == rect.X &&
//(2)            Y == rect.Y &&
//(2)            Width == rect.Width &&
//(2)            Height == rect.Height;
//(0) }

//(-1) BOOL Contains(IN REAL x,
//(1)               IN REAL y) const
//(2) {
//(2)     return x >= X && x < X+Width &&
//(2)            y >= Y && y < Y+Height;
//(0) }

//(1) BOOL Contains(IN const PointF& pt) const
//(2) {
//(2)     return Contains(pt.X, pt.Y);
//(0) }

//(1) BOOL Contains(IN const RectF& rect) const
//(2) {
//(2)     return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
//(2)            (Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
//(0) }

//(-1) VOID Inflate(IN REAL dx,
//(1)              IN REAL dy)
//(2) {
//(2)     X -= dx;
//(2)     Y -= dy;
//(2)     Width += 2*dx;
//(2)     Height += 2*dy;
//(0) }

//(1) VOID Inflate(IN const PointF& point)
//(2) {
//(2)     Inflate(point.X, point.Y);
//(0) }

//(1) BOOL Intersect(IN const RectF& rect)
//(2) {
//(2)     return Intersect(*this, *this, rect);
//(0) }

//(-1) static BOOL Intersect(OUT RectF& c,
//(-1)                       IN const RectF& a,
//(1)                       IN const RectF& b)
//(2) {
//(2)     REAL right = min(a.GetRight(), b.GetRight());
//(2)     REAL bottom = min(a.GetBottom(), b.GetBottom());
//(2)     REAL left = max(a.GetLeft(), b.GetLeft());
//(2)     REAL top = max(a.GetTop(), b.GetTop());

//(2)     c.X = left;
//(2)     c.Y = top;
//(2)     c.Width = right - left;
//(2)     c.Height = bottom - top;
//(2)     return !c.IsEmptyArea();
//(0) }

//(1) BOOL IntersectsWith(IN const RectF& rect) const
//(2) {
//(2)     return (GetLeft() < rect.GetRight() &&
//(2)             GetTop() < rect.GetBottom() &&
//(2)             GetRight() > rect.GetLeft() &&
//(2)             GetBottom() > rect.GetTop());
//(0) }

//(-1) static BOOL Union(OUT RectF& c,
//(-1)                   IN const RectF& a,
//(1)                   IN const RectF& b)
//(2) {
//(2)     REAL right = max(a.GetRight(), b.GetRight());
//(2)     REAL bottom = max(a.GetBottom(), b.GetBottom());
//(2)     REAL left = min(a.GetLeft(), b.GetLeft());
//(2)     REAL top = min(a.GetTop(), b.GetTop());

//(2)     c.X = left;
//(2)     c.Y = top;
//(2)     c.Width = right - left;
//(2)     c.Height = bottom - top;
//(2)     return !c.IsEmptyArea();
//(0) }

//(1) VOID Offset(IN const PointF& point)
//(2) {
//(2)     Offset(point.X, point.Y);
//(0) }

//(-1) VOID Offset(IN REAL dx,
//(1)             IN REAL dy)
//(2) {
//(2)     X += dx;
//(2)     Y += dy;
//(0) }



//--------------------------------------------------------------------------
// Represents a rectangle in a 2D coordinate system (integer coordinates)
//--------------------------------------------------------------------------

typedef struct _Rect
{
    INT X;
    INT Y;
    INT Width;
    INT Height;
} Rect;

//(1) Rect()
//(2) {
//(2)     X = Y = Width = Height = 0;
//(0) }

//(-1) Rect(IN INT x,
//(-1)      IN INT y,
//(-1)      IN INT width,
//(1)      IN INT height)
//(2) {
//(2)     X = x;
//(2)     Y = y;
//(2)     Width = width;
//(2)     Height = height;
//(0) }

//(-1) Rect(IN const Point& location,
//(1)      IN const Size& size)
//(2) {
//(2)     X = location.X;
//(2)     Y = location.Y;
//(2)     Width = size.Width;
//(2)     Height = size.Height;
//(0) }

//(1) Rect* Clone() const
//(2) {
//(2)     return new Rect(X, Y, Width, Height);
//(0) }

//(1) VOID GetLocation(OUT Point* point) const
//(2) {
//(2)     point->X = X;
//(2)     point->Y = Y;
//(0) }

//(1) VOID GetSize(OUT Size* size) const
//(2) {
//(2)     size->Width = Width;
//(2)     size->Height = Height;
//(0) }

//(1) VOID GetBounds(OUT Rect* rect) const
//(2) {
//(2)     rect->X = X;
//(2)     rect->Y = Y;
//(2)     rect->Width = Width;
//(2)     rect->Height = Height;
//(0) }

//(1) INT GetLeft() const
//(2) {
//(2)     return X;
//(0) }

//(1) INT GetTop() const
//(2) {
//(2)     return Y;
//(0) }

//(1) INT GetRight() const
//(2) {
//(2)     return X+Width;
//(0) }

//(1) INT GetBottom() const
//(2) {
//(2)     return Y+Height;
//(0) }

//(1) BOOL IsEmptyArea() const
//(2) {
//(2)     return (Width <= 0) || (Height <= 0);
//(0) }

//(1) BOOL Equals(IN const Rect & rect) const
//(2) {
//(2)     return X == rect.X &&
//(2)            Y == rect.Y &&
//(2)            Width == rect.Width &&
//(2)            Height == rect.Height;
//(0) }

//(-1) BOOL Contains(IN INT x,
//(1)               IN INT y) const
//(2) {
//(2)     return x >= X && x < X+Width &&
//(2)            y >= Y && y < Y+Height;
//(0) }

//(1) BOOL Contains(IN const Point& pt) const
//(2) {
//(2)     return Contains(pt.X, pt.Y);
//(0) }

//(1) BOOL Contains(IN Rect& rect) const
//(2) {
//(2)     return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
//(2)            (Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
//(0) }

//(-1) VOID Inflate(IN INT dx,
//(1)              IN INT dy)
//(2) {
//(2)     X -= dx;
//(2)     Y -= dy;
//(2)     Width += 2*dx;
//(2)     Height += 2*dy;
//(0) }

//(1) VOID Inflate(IN const Point& point)
//(2) {
//(2)     Inflate(point.X, point.Y);
//(0) }

//(1) BOOL Intersect(IN const Rect& rect)
//(2) {
//(2)     return Intersect(*this, *this, rect);
//(0) }

//(-1) static BOOL Intersect(OUT Rect& c,
//(-1)                       IN const Rect& a,
//(1)                       IN const Rect& b)
//(2) {
//(2)     INT right = min(a.GetRight(), b.GetRight());
//(2)     INT bottom = min(a.GetBottom(), b.GetBottom());
//(2)     INT left = max(a.GetLeft(), b.GetLeft());
//(2)     INT top = max(a.GetTop(), b.GetTop());

//(2)     c.X = left;
//(2)     c.Y = top;
//(2)     c.Width = right - left;
//(2)     c.Height = bottom - top;
//(2)     return !c.IsEmptyArea();
//(0) }

//(1) BOOL IntersectsWith(IN const Rect& rect) const
//(2) {
//(2)     return (GetLeft() < rect.GetRight() &&
//(2)             GetTop() < rect.GetBottom() &&
//(2)             GetRight() > rect.GetLeft() &&
//(2)             GetBottom() > rect.GetTop());
//(0) }

//(-1) static BOOL Union(OUT Rect& c,
//(-1)                   IN const Rect& a,
//(1)                   IN const Rect& b)
//(2) {
//(2)     INT right = max(a.GetRight(), b.GetRight());
//(2)     INT bottom = max(a.GetBottom(), b.GetBottom());
//(2)     INT left = min(a.GetLeft(), b.GetLeft());
//(2)     INT top = min(a.GetTop(), b.GetTop());

//(2)     c.X = left;
//(2)     c.Y = top;
//(2)     c.Width = right - left;
//(2)     c.Height = bottom - top;
//(2)     return !c.IsEmptyArea();
//(0) }

//(1) VOID Offset(IN const Point& point)
//(2) {
//(2)     Offset(point.X, point.Y);
//(0) }

//(-1) VOID Offset(IN INT dx,
//(1)             IN INT dy)
//(2) {
//(2)     X += dx;
//(2)     Y += dy;
//(0) }



typedef struct _PathData
{
    INT Count;
    PointF* Points;
} PathData;
//(1) PathData()
//(2) {
//(2)     Count = 0;
//(2)     Points = NULL;
//(2)     Types = NULL;
//(0) }

//(1) ~PathData()
//(2) {
//(2)     if (Points != NULL)
//(3)     {
//(3)         delete [] Points;
//(2)     }

//(2)     if (Types != NULL)
//(3)     {
//(3)         delete [] Types;
//(2)     }
//(0) }

//(0) PathData(const PathData &);
//(0) PathData& operator=(const PathData &);

//(0) __field_ecount_opt(Count) BYTE* Types;

typedef struct _CharacterRange
{
    INT First;
    INT Length;
} CharacterRange;
//(-1) CharacterRange(
//(-1)     INT first,
//(-1)     INT length
//(1) ) :
//(1)     First   (first),
//(1)     Length  (length)
//(0) {}

//(1) CharacterRange() : First(0), Length(0)
//(0) {}

//(1) CharacterRange & operator = (const CharacterRange &rhs)
//(2) {
//(2)     First  = rhs.First;
//(2)     Length = rhs.Length;
//(2)     return *this;
//(0) }


#endif // !_GDIPLUSTYPES_HPP

