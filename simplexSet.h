// pbrendel (c) 2021

#pragma once

#include "Core/ptr.h"
#include "Core/types.h"

class SimplexSet;
typedef uint Label;


class ConstSimplex
{
public:

	uint GetDimension() const;
	Label operator[]( uint index ) const;

protected:

	ConstSimplex( const SimplexSet *set, uint offset );

	const SimplexSet *m_set;
	uint m_offset;

	friend class SimplexSet;
};


class Simplex : public ConstSimplex
{
public:

	Label &operator[]( uint index );

private:

	Simplex( SimplexSet *set, uint offset );

	friend class SimplexSet;
};


class SimplexSet
{
public:

	enum : Label
	{
		INVALID_LABEL = static_cast<Label>( 0xFFFFFF ),
	};

	SimplexSet();
	SimplexSet( uint dim, uint capacity );

	void Init( uint dim, uint capacity );
	void Resize( uint size );

	uint GetDimension() const
	{
		return m_dim;
	}

	uint GetSize() const
	{
		return m_size;
	}

	Simplex PushBack();
	void PopBack();
	void Remove( uint index );

	Simplex operator[]( uint index );
	ConstSimplex operator[]( uint index ) const;

	void MakeClean();

private:

	uint GetSimplexOffset( uint index ) const;
	void Resize();

	o::Ptr<Label> m_labels;
	uint m_dim;
	uint m_capacity;
	uint m_size;
	bool m_dirty;

	friend class Simplex;
	friend class ConstSimplex;
};
