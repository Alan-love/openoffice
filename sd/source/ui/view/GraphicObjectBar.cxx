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
#include "precompiled_sd.hxx"

#include "GraphicObjectBar.hxx"

#include <limits.h>
#include <vcl/msgbox.hxx>
#include <svl/whiter.hxx>
#include <svl/itempool.hxx>
#include <sfx2/app.hxx>
#include <sfx2/shell.hxx>
#include <svx/svxids.hrc>
#include <sfx2/request.hxx>
#include <sfx2/basedlgs.hxx>
#include <svx/svdograf.hxx>
#include <svx/grfflt.hxx>
#include <svl/aeitem.hxx>
#include <svx/grafctrl.hxx>


#include <sfx2/objface.hxx>

#include "app.hrc"
#include "res_bmp.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "DrawDocShell.hxx"
#include "ViewShell.hxx"
#include "Window.hxx"
#include "drawview.hxx"
#include "sdresid.hxx"
#include "drawdoc.hxx"

using namespace sd;
#define GraphicObjectBar
#include "sdslots.hxx"

namespace sd {



// -----------------------
// - GraphicObjectBar -
// -----------------------

SFX_IMPL_INTERFACE( GraphicObjectBar, SfxShell, SdResId( STR_GRAFOBJECTBARSHELL ) )
{
}

// -----------------------------------------------------------------------------

TYPEINIT1( GraphicObjectBar, SfxShell );

// -----------------------------------------------------------------------------

GraphicObjectBar::GraphicObjectBar (
    ViewShell* pSdViewShell,
    ::sd::View* pSdView )
    : SfxShell (pSdViewShell->GetViewShell()),
      mpView	 ( pSdView ),
      mpViewSh ( pSdViewShell ),
      nMappedSlotFilter ( SID_GRFFILTER_INVERT )
{
	DrawDocShell* pDocShell = mpViewSh->GetDocSh();

	SetPool( &pDocShell->GetPool() );
	SetUndoManager( pDocShell->GetUndoManager() );
	SetRepeatTarget( mpView );
	SetHelpId( SD_IF_SDDRAWGRAFOBJECTBAR );
	SetName( String( RTL_CONSTASCII_USTRINGPARAM( "Graphic objectbar" )));
}

// -----------------------------------------------------------------------------

GraphicObjectBar::~GraphicObjectBar()
{
	SetRepeatTarget( NULL );
}

// -----------------------------------------------------------------------------

void GraphicObjectBar::GetAttrState( SfxItemSet& rSet )
{
	if( mpView )
		SvxGrafAttrHelper::GetGrafAttrState( rSet, *mpView );
}

// -----------------------------------------------------------------------------

void GraphicObjectBar::Execute( SfxRequest& rReq )
{
	if( mpView )
	{
		SvxGrafAttrHelper::ExecuteGrafAttr( rReq, *mpView );
		Invalidate();
	}
}

// -----------------------------------------------------------------------------

void GraphicObjectBar::GetFilterState( SfxItemSet& rSet )
{
	const SdrMarkList&	rMarkList = mpView->GetMarkedObjectList();
	sal_Bool				bEnable = sal_False;

	if( rMarkList.GetMarkCount() == 1 )
	{
		SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

		if( pObj && pObj->ISA( SdrGrafObj ) && ( ( (SdrGrafObj*) pObj )->GetGraphicType() == GRAPHIC_BITMAP ) )
			bEnable = sal_True;
	}

	if( !bEnable )
		SvxGraphicFilter::DisableGraphicFilterSlots( rSet );
}

// -----------------------------------------------------------------------------

void GraphicObjectBar::ExecuteFilter( SfxRequest& rReq )
{
	const SdrMarkList& rMarkList = mpView->GetMarkedObjectList();

	if( rMarkList.GetMarkCount() == 1 )
	{
		SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

		if( pObj && pObj->ISA( SdrGrafObj ) && ( (SdrGrafObj*) pObj )->GetGraphicType() == GRAPHIC_BITMAP )
		{
			GraphicObject aFilterObj( ( (SdrGrafObj*) pObj )->GetGraphicObject() );

			if( SVX_GRAPHICFILTER_ERRCODE_NONE == 
				SvxGraphicFilter::ExecuteGrfFilterSlot( rReq, aFilterObj ) )
			{
				SdrPageView* pPageView = mpView->GetSdrPageView();

				if( pPageView )
				{
					SdrGrafObj*	pFilteredObj = (SdrGrafObj*) pObj->Clone();
					String		aStr( mpView->GetDescriptionOfMarkedObjects() );

					aStr.Append( sal_Unicode(' ') );
					aStr.Append( String( SdResId( STR_UNDO_GRAFFILTER ) ) );
					mpView->BegUndo( aStr );
					pFilteredObj->SetGraphicObject( aFilterObj );
					::sd::View* const pView = mpView;
					pView->ReplaceObjectAtView( pObj, *pPageView, pFilteredObj );
					pView->EndUndo();
					return;
				}
			}
		}
	}

	Invalidate();
}

} // end of namespace sd
