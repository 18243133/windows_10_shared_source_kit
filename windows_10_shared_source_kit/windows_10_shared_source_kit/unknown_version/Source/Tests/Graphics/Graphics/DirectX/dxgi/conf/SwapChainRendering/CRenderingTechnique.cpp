//////////////////////////////////////////////////////////////////////
// File:  CRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Purpose:
// Implementation of a base class for rendering techniques.
//
// History:
// 24 Aug 2015  SEdmison  Factored out base class object.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "HelperMacros.h"
#include "RenderingTypes.hpp"


/////////////////////////////////////////////////////////////////
//
// CRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CRenderingTechnique::CRenderingTechnique
(
	_In_ ISimpleLog * pLogger
)
:
	m_pLogger( nullptr )
{
	SetLogger( pLogger );
};


///////////////////////////////////////////////////////
// Function:  ~CRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CRenderingTechnique::~CRenderingTechnique()
{
	SetLogger( nullptr );
};


////////////////////////////////////////////////////////////
//
// Initialization/cleanup
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  SetLogger
//
// Purpose:
// Assigns a logger to the rendering technique.
///////////////////////////////////////////////////////
void CRenderingTechnique::SetLogger
(
	_In_ ISimpleLog * pLogger
)
{
	SAFE_RELEASE( m_pLogger );

	m_pLogger = pLogger;

	if ( m_pLogger != nullptr )
	{
		m_pLogger->AddRef();
	};
};