/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmlscript.hxx"
#include <rtl/memory.h>

#include <cppuhelper/implbase1.hxx>
#include <xmlscript/xml_helper.hxx>


using namespace rtl;
using namespace osl;
using namespace com::sun::star;
using namespace com::sun::star::uno;


namespace xmlscript
{

//==================================================================================================
class BSeqInputStream
	: public ::cppu::WeakImplHelper1< io::XInputStream >
{
	ByteSequence _seq;
	sal_Int32 _nPos;
	
public:
	inline BSeqInputStream( ByteSequence const & rSeq )
		SAL_THROW( () )
		: _seq( rSeq )
		, _nPos( 0 )
		{}
	
	// XInputStream
    virtual sal_Int32 SAL_CALL readBytes(
		Sequence< sal_Int8 > & rData, sal_Int32 nBytesToRead )
		throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException);
	virtual sal_Int32 SAL_CALL readSomeBytes(
		Sequence< sal_Int8 > & rData, sal_Int32 nMaxBytesToRead )
		throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException);
    virtual void SAL_CALL skipBytes(
		sal_Int32 nBytesToSkip )
		throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException);
    virtual sal_Int32 SAL_CALL available()
		throw (io::NotConnectedException, io::IOException, RuntimeException);
    virtual void SAL_CALL closeInput()
		throw (io::NotConnectedException, io::IOException, RuntimeException);
};
//__________________________________________________________________________________________________
sal_Int32 BSeqInputStream::readBytes(
	Sequence< sal_Int8 > & rData, sal_Int32 nBytesToRead )
	throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException)
{
	nBytesToRead = ((nBytesToRead > _seq.getLength() - _nPos)
					? _seq.getLength() - _nPos
					: nBytesToRead);
	
	ByteSequence aBytes( _seq.getConstArray() + _nPos, nBytesToRead );
	rData = toUnoSequence( aBytes );
	_nPos += nBytesToRead;
	return nBytesToRead;
}
//__________________________________________________________________________________________________
sal_Int32 BSeqInputStream::readSomeBytes(
	Sequence< sal_Int8 > & rData, sal_Int32 nMaxBytesToRead )
	throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException)
{
	return readBytes( rData, nMaxBytesToRead );
}
//__________________________________________________________________________________________________
void BSeqInputStream::skipBytes(
	sal_Int32 /*nBytesToSkip*/ )
	throw (io::NotConnectedException, io::BufferSizeExceededException, io::IOException, RuntimeException)
{
}
//__________________________________________________________________________________________________
sal_Int32 BSeqInputStream::available()
	throw (io::NotConnectedException, io::IOException, RuntimeException)
{
	return (_seq.getLength() - _nPos);
}
//__________________________________________________________________________________________________
void BSeqInputStream::closeInput()
	throw (io::NotConnectedException, io::IOException, RuntimeException)
{
}

//##################################################################################################

//==================================================================================================
class BSeqOutputStream
	: public ::cppu::WeakImplHelper1< io::XOutputStream >
{
	ByteSequence * _seq;
	
public:
	inline BSeqOutputStream( ByteSequence * seq )
		SAL_THROW( () )
		: _seq( seq )
		{}
	
	// XOutputStream
    virtual void SAL_CALL writeBytes(
		Sequence< sal_Int8 > const & rData )
		throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException);
    virtual void SAL_CALL flush()
		throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException);
    virtual void SAL_CALL closeOutput()
		throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException);
};
//__________________________________________________________________________________________________
void BSeqOutputStream::writeBytes( Sequence< sal_Int8 > const & rData )
	throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException)
{
	sal_Int32 nPos = _seq->getLength();
	_seq->realloc( nPos + rData.getLength() );
	::rtl_copyMemory( (char *)_seq->getArray() + nPos,
					  (char const *)rData.getConstArray(),
					  rData.getLength() );
}
//__________________________________________________________________________________________________
void BSeqOutputStream::flush()
	throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException)
{
}
//__________________________________________________________________________________________________
void BSeqOutputStream::closeOutput()
	throw (io::NotConnectedException, io::BufferSizeExceededException, RuntimeException)
{
}

//##################################################################################################

//==================================================================================================
Reference< io::XInputStream > SAL_CALL createInputStream( ByteSequence const & rInData )
	SAL_THROW( () )
{
	return new BSeqInputStream( rInData );
}

//==================================================================================================
Reference< io::XOutputStream > SAL_CALL createOutputStream( ByteSequence * pOutData )
	SAL_THROW( () )
{
	return new BSeqOutputStream( pOutData );
}

}
