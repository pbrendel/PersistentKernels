// pbrendel (c) 2021

#include "simplexSet.h"
#include "Core/assert.h"
#include "Core/defs.h"

#include <algorithm>
#include <cstring>


ConstSimplex::ConstSimplex( const SimplexSet *set, uint offset )
	: m_set( set )
	, m_offset( offset )
{}


uint ConstSimplex::GetDimension() const
{
	return m_set->GetDimension();
}


Label ConstSimplex::operator[]( uint index ) const
{
	assert( index <= GetDimension() );
	return m_set->m_labels[m_offset + index];
}


Simplex::Simplex( SimplexSet *set, uint offset )
	: ConstSimplex( set, offset )
{
}


Label &Simplex::operator[]( uint index )
{
	assert( index <= GetDimension() );
	return const_cast<SimplexSet *>( m_set )->m_labels[m_offset + index];
}

////////////////////////////////////////////////////////////////////////////////

SimplexSet::SimplexSet()
	: m_labels( nullptr )
	, m_capacity( 0 )
	, m_size( 0 )
	, m_dirty( false )
{}


SimplexSet::SimplexSet( uint dim, uint capacity )
	: m_dim( dim )
	, m_capacity( capacity )
	, m_size( 0 )
	, m_dirty( false )
{
	assert( m_capacity > 0 );
	const uint labelsSize = GetSimplexOffset( m_capacity );
	m_labels = new Label[labelsSize];
	std::fill_n( m_labels.Get(), labelsSize, INVALID_LABEL );
}


void SimplexSet::Init( uint dim, uint capacity )
{
	assert( m_capacity == 0 );
	m_dim = dim;
	m_capacity = capacity;
	const uint labelsSize = GetSimplexOffset( m_capacity );
	m_labels = new Label[labelsSize];
	std::fill_n( m_labels.Get(), labelsSize, INVALID_LABEL );
	m_dirty = false;
}


void SimplexSet::Resize( uint size )
{
	m_size = size;
}


uint SimplexSet::GetSimplexOffset( uint index ) const
{
	return index * ( m_dim + 1 );
}


Simplex SimplexSet::PushBack()
{
	if ( m_size == m_capacity )
	{
		Resize();
	}
	const uint offset = GetSimplexOffset( m_size++ );
	return Simplex( this, offset );
}


void SimplexSet::PopBack()
{
	assert( m_size > 0 );
	m_size--;
}


void SimplexSet::Remove( uint index )
{
	assert( m_size > 0 );
	const uint offset = GetSimplexOffset( index );
	m_labels[offset] = INVALID_LABEL;
	m_size--;
	m_dirty |= ( index < m_size );
}


Simplex SimplexSet::operator[]( uint index )
{
	assert( index < m_size );
	const uint offset = GetSimplexOffset( index );
	return Simplex( this, offset );
}


ConstSimplex SimplexSet::operator[]( uint index ) const
{
	assert( index < m_size );
	const uint offset = GetSimplexOffset( index );
	return ConstSimplex( this, offset );
}


void SimplexSet::Resize()
{
	const uint newCapacity = m_capacity == 0 ? 2 : m_capacity * 2;
	const uint newLabelsSize = GetSimplexOffset( newCapacity );
	const uint oldLabelsSize = GetSimplexOffset( m_capacity );
	Label *newLabels = new Label[newLabelsSize];
	memcpy( newLabels, m_labels.Get(), oldLabelsSize * sizeof( Label ) );
	m_labels = newLabels;
	m_capacity = newCapacity;
}


void SimplexSet::MakeClean()
{
	uint srcOffset = 0;
	uint dstOffset = 0;
	Label *labels = m_labels.Get();
	const uint simplexSize = m_dim + 1;
	const uint totalSize = m_size * simplexSize;
	while ( srcOffset < totalSize )
	{
		while ( srcOffset < totalSize && labels[srcOffset] == INVALID_LABEL )
		{
			srcOffset += simplexSize;
		}
		const uint srcOffsetBegin = srcOffset;
		while ( srcOffset < totalSize && labels[srcOffset] != INVALID_LABEL )
		{
			srcOffset += simplexSize;
		}
		if ( srcOffsetBegin != dstOffset && srcOffsetBegin != srcOffset )
		{
			const uint toMove = ( srcOffset - srcOffsetBegin );
			memmove( labels + dstOffset, labels + srcOffsetBegin, toMove );
			dstOffset += toMove;
		}
	}
	m_size = dstOffset / simplexSize;
	m_dirty = false;
}