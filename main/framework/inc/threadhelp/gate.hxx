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



#ifndef __FRAMEWORK_THREADHELP_GATE_HXX_
#define __FRAMEWORK_THREADHELP_GATE_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/inoncopyable.h>
#include <threadhelp/igate.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <osl/mutex.hxx>
#include <osl/conditn.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short          implement a gate to block multiple threads at same time or unblock all
	@descr			A gate can be used as a negative-condition! You can open a "door" - wait() will not block ...
					or you can close it - wait() blocks till open() is called again.
					As a special feature you can open the gate a little bit by sing openGap().
					Then all currently waiting threads are running immediately - but new ones are blocked!

	@attention		To prevent us against wrong using, the default ctor, copy ctor and the =operator are marked private!

    @implements     IGate
    @base           IGate
                    INonCopyable

	@devstatus		ready to use
*//*-*************************************************************************************************************/
class Gate : public  IGate
           , private INonCopyable
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------
	public:

		/*-****************************************************************************************************//**
			@short		ctor
			@descr		These initialize the object right as an open gate.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
        inline Gate()
            :   m_bClosed   ( sal_False )
            ,   m_bGapOpen  ( sal_False )
        {
            open();
        }

		/*-****************************************************************************************************//**
			@short		dtor
			@descr		Is user forget it - we open the gate ...
						blocked threads can running ... but I don't know
						if it's right - we are destroyed yet!?

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
        inline virtual ~Gate()
        {
            open();
        }

		/*-****************************************************************************************************//**
            @interface  IGate
			@short		open the gate
			@descr		A wait() call will not block then.

			@seealso	method close()

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
        inline virtual void open()
        {
            // We must safe access to our internal member!
            ::osl::MutexGuard aLock( m_aAccessLock );
            // Set condition -> wait don't block any longer -> gate is open
            m_aPassage.set();
            // Check if operation was successful!
            // Check returns false if condition isn't set => m_bClosed will be true then => we must return false; opening failed
            m_bClosed = ( m_aPassage.check() == sal_False );
        }

		/*-****************************************************************************************************//**
            @interface  IGate
			@short		close the gate
			@descr		A wait() call will block then.

			@seealso	method open()

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
        inline virtual void close()
        {
            // We must safe access to our internal member!
            ::osl::MutexGuard aLock( m_aAccessLock );
            // Reset condition -> wait blocks now -> gate is closed
            m_aPassage.reset();
            // Check if operation was successful!
            // Check returns false if condition was reseted => m_bClosed will be true then => we can return true; closing ok
            m_bClosed = ( m_aPassage.check() == sal_False );
        }

		/*-****************************************************************************************************//**
            @interface  IGate
			@short		open gate for current waiting threads
			@descr		All current waiting threads stand in wait() at line "m_aPassage.wait()" ...
						With this call you can open the passage for these waiting ones.
						The "gap" is closed by any new thread which call wait() automatically!

			@seealso	method wait()
			@seealso	method open()

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
        inline virtual void openGap()
        {
            // We must safe access to our internal member!
            ::osl::MutexGuard aLock( m_aAccessLock );
            // Open passage for current waiting threads.
            m_aPassage.set();
            // Check state of condition.
            // If condition is set check() returns true => m_bGapOpen will be true too => we can use it as return value.
            m_bGapOpen = ( m_aPassage.check() == sal_True );
        }

		/*-****************************************************************************************************//**
            @interface  IGate
			@short		must be called to pass the gate
			@descr		If gate "open"   => wait() will not block.
						If gate "closed" => wait() will block till somewhere open it again.
						If gap  "open"   => currently waiting threads unblocked, new ones blocked

			@seealso	method wait()
			@seealso	method open()

			@param		"pTimeOut", optional parameter to wait a certain time
			@return		true, if wait was successful (gate was opened)
						false, if condition has an error or timeout was reached!

			@onerror	We return false.
		*//*-*****************************************************************************************************/
        inline virtual sal_Bool wait( const TimeValue* pTimeOut = NULL )
        {
            // We must safe access to our internal member!
            ::osl::ClearableMutexGuard aLock( m_aAccessLock );
            // If gate not closed - caller can pass it.
            sal_Bool bSuccessful = sal_True;
            if( m_bClosed == sal_True )
            {
                // Otherwise first new thread must close an open gap!
                if( m_bGapOpen == sal_True )
                {
                    m_bGapOpen = sal_False;
                    m_aPassage.reset();
                }
                // Then we must release used access lock -
                // because next call will block ...
                // and if we hold the access lock nobody else can use this object without a dadlock!
                aLock.clear();
                // Wait for opening gate ...
                bSuccessful = ( m_aPassage.wait( pTimeOut ) == ::osl::Condition::result_ok );
            }

            return bSuccessful;
        }

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------
	private:

		::osl::Mutex		m_aAccessLock	;
		::osl::Condition	m_aPassage		;
		sal_Bool			m_bClosed		;
		sal_Bool			m_bGapOpen		;

};		//	class Gate

}		//	namespace framework

#endif	//	#ifndef __FRAMEWORK_THREADHELP_GATE_HXX_
