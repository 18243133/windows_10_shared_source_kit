#include "UAV.h"

BEGIN_NAMED_VALUES(RESOURCE_TYPE)
	NAMED_VALUE("ByteAddressBuffer", RESOURCE_BYTEADDRESSBUFFER)
	NAMED_VALUE("StructuredBuffer", RESOURCE_STRUCTUREDBUFFER)
	NAMED_VALUE("StructuredBuffer1Float", RESOURCE_STRUCTUREDBUFFER_1FLOAT)
	NAMED_VALUE("StructuredBuffer4UInt", RESOURCE_STRUCTUREDBUFFER_4UINT)
	NAMED_VALUE("StructuredBuffer2SInt", RESOURCE_STRUCTUREDBUFFER_2SINT)
	NAMED_VALUE("StructuredBufferMix", RESOURCE_STRUCTUREDBUFFER_MIX)
	NAMED_VALUE("RWBuffer", RESOURCE_RWBUFFER)
	NAMED_VALUE("RWBufferSInt", RESOURCE_RWBUFFER_SINT)
	NAMED_VALUE("RWBufferUInt", RESOURCE_RWBUFFER_UINT)
	NAMED_VALUE("RWBufferUInt2", RESOURCE_RWBUFFER_UINT2)
	NAMED_VALUE("RWBufferFloat", RESOURCE_RWBUFFER_FLOAT)
	NAMED_VALUE("RWBufferFloat4", RESOURCE_RWBUFFER_FLOAT4)
	NAMED_VALUE("RWByteAddressBuffer", RESOURCE_RWBYTEADDRESSBUFFER)
	NAMED_VALUE("RWStructuredBuffer", RESOURCE_RWSTRUCTUREDBUFFER)
	NAMED_VALUE("RWStructuredBuffer1Float", RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT)
	NAMED_VALUE("RWStructuredBuffer4UInt", RESOURCE_RWSTRUCTUREDBUFFER_4UINT)
	NAMED_VALUE("RWStructuredBuffer2SInt", RESOURCE_RWSTRUCTUREDBUFFER_2SINT)
	NAMED_VALUE("RWStructuredBufferMix", RESOURCE_RWSTRUCTUREDBUFFER_MIX)
	NAMED_VALUE("RWTexture1D", RESOURCE_RWTEXTURE1D)
	NAMED_VALUE("RWTexture1DArray", RESOURCE_RWTEXTURE1D_ARRAY)
	NAMED_VALUE("RWTexture2D", RESOURCE_RWTEXTURE2D)
	NAMED_VALUE("RWTexture2DArray", RESOURCE_RWTEXTURE2D_ARRAY)
	NAMED_VALUE("RWTexture3D", RESOURCE_RWTEXTURE3D)
END_NAMED_VALUES(RESOURCE_TYPE)

BEGIN_NAMED_VALUES(UAV_OPERATION)
	NAMED_VALUE("Read", UAV_OP_READ)
	NAMED_VALUE("ReadOutOfBounds", UAV_OP_READ_OOB)
	NAMED_VALUE("ReadTypedUAVAdditionalFormats", UAV_OP_READ_AF)
	NAMED_VALUE("Write", UAV_OP_WRITE)
END_NAMED_VALUES(UAV_OPERATION)

TEST_RESULT CUAVTest::CreateShaderResource()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	
	D3D11_BUFFER_DESC descBuffer;
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ID3D11Resource *pResource = NULL;

	bool bStructured = m_SrcResourceType == RESOURCE_STRUCTUREDBUFFER || m_SrcResourceType == RESOURCE_STRUCTUREDBUFFER_1FLOAT  || m_SrcResourceType == RESOURCE_STRUCTUREDBUFFER_2SINT || m_SrcResourceType == RESOURCE_STRUCTUREDBUFFER_4UINT || m_SrcResourceType == RESOURCE_STRUCTUREDBUFFER_MIX;	

	switch( m_SrcResourceType )
	{
	case RESOURCE_BYTEADDRESSBUFFER:
	case RESOURCE_STRUCTUREDBUFFER:
	case RESOURCE_STRUCTUREDBUFFER_1FLOAT:
	case RESOURCE_STRUCTUREDBUFFER_2SINT:
	case RESOURCE_STRUCTUREDBUFFER_4UINT:
	case RESOURCE_STRUCTUREDBUFFER_MIX:
		descBuffer.ByteWidth = m_uBufferSize;
		descBuffer.Usage = D3D11_USAGE_DYNAMIC;
		descBuffer.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		descBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		descBuffer.MiscFlags = bStructured ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		descBuffer.StructureByteStride = m_uBufferStride;

		D3D11_SUBRESOURCE_DATA subresData;
		subresData.pSysMem = (void*)m_pInitialData;

		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &descBuffer, &subresData, &m_pSrcBuffer ) ) )
		{
			WriteToLog(_T("CUAVTest::CreateShaderResource() - Failed to create SR buffer, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		pResource = m_pSrcBuffer;
		break;
	}

	D3D11_SRV_DIMENSION viewDim = GetSRVDimension(m_SrcResourceType);	 
	descSRV.Format = bStructured ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS;
	descSRV.ViewDimension = viewDim;
	switch( viewDim )
	{
	case D3D11_SRV_DIMENSION_BUFFEREX:
		descSRV.BufferEx.FirstElement = 0;	
		descSRV.BufferEx.NumElements = m_uBufferSize / m_uBufferElementWidth;	
		descSRV.BufferEx.Flags = m_SrcResourceType == RESOURCE_BYTEADDRESSBUFFER ? D3D11_BUFFEREX_SRV_FLAG_RAW : 0;
		break;
	}

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( pResource, &descSRV, &m_pSRV ) ) )
	{
		WriteToLog(_T("CUAVTest::CreateShaderResource() - Failed to create SRV, hr=%s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return tr;
}

D3D11_SRV_DIMENSION CUAVTest::GetSRVDimension( RESOURCE_TYPE resType )
{
	D3D11_SRV_DIMENSION result = D3D11_SRV_DIMENSION_UNKNOWN;

	switch( resType )
	{
	case RESOURCE_BYTEADDRESSBUFFER:
	case RESOURCE_STRUCTUREDBUFFER:
	case RESOURCE_STRUCTUREDBUFFER_1FLOAT:
	case RESOURCE_STRUCTUREDBUFFER_2SINT:
	case RESOURCE_STRUCTUREDBUFFER_4UINT:
	case RESOURCE_STRUCTUREDBUFFER_MIX:
		result = D3D11_SRV_DIMENSION_BUFFEREX;
		break;
	case RESOURCE_RWBUFFER:
		result = D3D11_SRV_DIMENSION_BUFFER;
		break;
	case RESOURCE_RWTEXTURE1D:
		result = D3D11_SRV_DIMENSION_TEXTURE1D;
		break;
	case RESOURCE_RWTEXTURE1D_ARRAY:
		result = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		break;
	case RESOURCE_RWTEXTURE2D:
		result = D3D11_SRV_DIMENSION_TEXTURE2D;
		break;
	case RESOURCE_RWTEXTURE2D_ARRAY:
		result = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		break;
	case RESOURCE_RWTEXTURE3D:
		result = D3D11_SRV_DIMENSION_TEXTURE3D;
		break;
	}

	return result;
}

D3D11_UAV_DIMENSION CUAVTest::GetUAVDimension( RESOURCE_TYPE resType )
{
	D3D11_UAV_DIMENSION result = D3D11_UAV_DIMENSION_UNKNOWN;

	switch( resType )
	{
	case RESOURCE_BYTEADDRESSBUFFER:
	case RESOURCE_STRUCTUREDBUFFER:
	case RESOURCE_STRUCTUREDBUFFER_1FLOAT:
	case RESOURCE_STRUCTUREDBUFFER_2SINT:
	case RESOURCE_STRUCTUREDBUFFER_4UINT:
	case RESOURCE_STRUCTUREDBUFFER_MIX:
	case RESOURCE_RWBUFFER:
	case RESOURCE_RWBUFFER_FLOAT:
	case RESOURCE_RWBUFFER_FLOAT4:
	case RESOURCE_RWBUFFER_SINT:
	case RESOURCE_RWBUFFER_UINT:
	case RESOURCE_RWBUFFER_UINT2:
	case RESOURCE_RWBYTEADDRESSBUFFER:
	case RESOURCE_RWSTRUCTUREDBUFFER:
	case RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT:
	case RESOURCE_RWSTRUCTUREDBUFFER_2SINT:
	case RESOURCE_RWSTRUCTUREDBUFFER_4UINT:
	case RESOURCE_RWSTRUCTUREDBUFFER_MIX:
		result = D3D11_UAV_DIMENSION_BUFFER;
		break;
	case RESOURCE_RWTEXTURE1D:
		result = D3D11_UAV_DIMENSION_TEXTURE1D;
		break;
	case RESOURCE_RWTEXTURE1D_ARRAY:
		result = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
		break;
	case RESOURCE_RWTEXTURE2D:
		result = D3D11_UAV_DIMENSION_TEXTURE2D;
		break;
	case RESOURCE_RWTEXTURE2D_ARRAY:
		result = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		break;
	case RESOURCE_RWTEXTURE3D:
		result = D3D11_UAV_DIMENSION_TEXTURE3D;
		break;
	}

	return result;
}

TEST_RESULT CUAVTest::SetupTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	memset(m_pInitialData, 0, MAX_RESULT_SIZE);
	memset(m_pExpectedResults, 0, MAX_RESULT_SIZE);
	memset(m_pComputedResults, 0, MAX_RESULT_SIZE);

	CreateInitialData();

	if( (tr = CreateShaderResource()) != RESULT_PASS )
		goto Done;

	if( (tr = CreateUAV()) != RESULT_PASS )
		goto Done;

	if( (tr = CreateShader()) != RESULT_PASS )
		goto Done;

Done:
	return tr;
}

void CUAVTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pUAV);
	SAFE_RELEASE(m_pSrcBuffer);
	SAFE_RELEASE(m_pUAVResource);
	SAFE_RELEASE(m_pUAVResourceStaging);
	SAFE_RELEASE(m_pComputeShader);
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);
}