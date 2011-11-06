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



#include "precompiled_sd.hxx"

#include "ToolPanelUIElement.hxx"
#include "MethodGuard.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/ui/UIElementType.hpp>
#include <com/sun/star/lang/XComponent.hpp>
/** === end UNO includes === **/

#include <tools/diagnose_ex.h>

//......................................................................................................................
namespace sd { namespace toolpanel
{
//......................................................................................................................

	/** === begin UNO using === **/
	using ::com::sun::star::uno::Reference;
	using ::com::sun::star::uno::XInterface;
	using ::com::sun::star::uno::UNO_QUERY;
	using ::com::sun::star::uno::UNO_QUERY_THROW;
	using ::com::sun::star::uno::UNO_SET_THROW;
	using ::com::sun::star::uno::Exception;
	using ::com::sun::star::uno::RuntimeException;
	using ::com::sun::star::uno::Any;
	using ::com::sun::star::uno::makeAny;
	using ::com::sun::star::uno::Sequence;
	using ::com::sun::star::uno::Type;
    using ::com::sun::star::frame::XFrame;
    using ::com::sun::star::lang::XComponent;
    using ::com::sun::star::ui::XToolPanel;
    using ::com::sun::star::lang::DisposedException;
	/** === end UNO using === **/
    namespace UIElementType = ::com::sun::star::ui::UIElementType;

    typedef MethodGuard< ToolPanelUIElement > UIElementMethodGuard;

	//==================================================================================================================
	//= ToolPanelUIElement
	//==================================================================================================================
	//------------------------------------------------------------------------------------------------------------------
    ToolPanelUIElement::ToolPanelUIElement( const Reference< XFrame >& i_rFrame, const ::rtl::OUString& i_rResourceURL,
            const Reference< XToolPanel >& i_rToolPanel )
        :ToolPanelUIElement_Base( m_aMutex )
        ,m_xFrame( i_rFrame )
        ,m_sResourceURL( i_rResourceURL )
        ,m_xToolPanel( i_rToolPanel )
    {
    }

	//------------------------------------------------------------------------------------------------------------------
    ToolPanelUIElement::~ToolPanelUIElement()
    {
    }

    //------------------------------------------------------------------------------------------------------------------
    void ToolPanelUIElement::checkDisposed()
    {
        if ( !m_xToolPanel.is() )
            throw DisposedException( ::rtl::OUString(), *this );
    }

    //------------------------------------------------------------------------------------------------------------------
    Reference< XFrame > SAL_CALL ToolPanelUIElement::getFrame() throw (RuntimeException)
    {
        UIElementMethodGuard aGuard( *this );
        return m_xFrame;
    }
    
    //------------------------------------------------------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ToolPanelUIElement::getResourceURL() throw (RuntimeException)
    {
        UIElementMethodGuard aGuard( *this );
        return m_sResourceURL;
    }
    
    //------------------------------------------------------------------------------------------------------------------
    ::sal_Int16 SAL_CALL ToolPanelUIElement::getType() throw (RuntimeException)
    {
        UIElementMethodGuard aGuard( *this );
        return UIElementType::TOOLPANEL;
    }
    
    //------------------------------------------------------------------------------------------------------------------
    Reference< XInterface > SAL_CALL ToolPanelUIElement::getRealInterface(  ) throw (RuntimeException)
    {
        UIElementMethodGuard aGuard( *this );
        return m_xToolPanel.get();
    }

    //------------------------------------------------------------------------------------------------------------------
    void SAL_CALL ToolPanelUIElement::disposing()
    {
        try
        {
            Reference< XComponent > xPanelComponent( m_xToolPanel, UNO_QUERY_THROW );
            xPanelComponent->dispose();
        }
        catch( const Exception& )
        {
        	DBG_UNHANDLED_EXCEPTION();
        }
    }

//......................................................................................................................
} } // namespace sd::toolpanel
//......................................................................................................................
