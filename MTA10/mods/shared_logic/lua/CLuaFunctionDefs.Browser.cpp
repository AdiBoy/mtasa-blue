/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Browser.cpp
*  PURPOSE:     Lua function definitions class
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateBrowser ( lua_State* luaVM )
{
//  texture createBrowser ( int width, int height, bool isLocal [, bool transparent = false] )
    CVector2D vecSize; bool bIsLocal; bool bTransparent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecSize );
    argStream.ReadBool ( bIsLocal );
    argStream.ReadBool ( bTransparent, false );
    
    if ( !argStream.HasErrors () )
    {
        if ( !bIsLocal && !g_pCore->GetWebCore()->CanLoadRemotePages () )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pParentResource = pLuaMain->GetResource ();

            CClientWebBrowser* pBrowserTexture = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateWebBrowser ( (int)vecSize.fX, (int)vecSize.fY, bIsLocal, bTransparent );
            if ( pBrowserTexture )
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pBrowserTexture->SetParent ( pParentResource->GetResourceDynamicEntity () );

                // Set our owner resource
                pBrowserTexture->SetResource ( pParentResource );
            }
            lua_pushelement ( luaVM, pBrowserTexture );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::RequestBrowserDomains ( lua_State* luaVM )
{
//  bool requestBrowserDomains ( table domains, bool isURL )
    std::vector<SString> pages; bool bIsURL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadStringTable ( pages );
    argStream.ReadBool ( bIsURL, false );

    if ( !argStream.HasErrors () )
    {
        // Convert to domains if we got a list of URLs
        if ( bIsURL )
        {
            for ( auto& strURL : pages )
            {
                strURL = g_pCore->GetWebCore ()->GetDomainFromURL ( strURL );
            }
        }

        g_pCore->GetWebCore ()->RequestPages ( pages );
        // Todo: Add a callback or event to check if the pagerequest dialog was successfully done
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::LoadBrowserURL ( lua_State* luaVM )
{
//  bool loadBrowserURL ( browser webBrowser, string url [, string postData = "", bool postURLEncoded = true ] )
    CClientWebBrowser* pWebBrowser; SString strURL; SString strPostData; bool bURLEncoded;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strURL );
    argStream.ReadString ( strPostData, "" );
    argStream.ReadBool ( bURLEncoded, true );

    if ( !argStream.HasErrors () )
    {
        // Are we dealing with a remote website?
        if ( strURL.substr ( 0, 7 ) == "http://" || strURL.substr ( 0, 8 ) == "https://" )
        {
            lua_pushboolean ( luaVM, pWebBrowser->LoadURL ( strURL, true, strPostData, bURLEncoded ) );
            return 1;
        }

        // Are we dealing with a local website? If so, parse resource path. Otherwise, return false and load nothing
        // Todo: Add an ACL right which is necessary to load local websites or websites in general
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            SString strAbsPath;
            CResource* pResource = pLuaMain->GetResource ();
            if ( CResourceManager::ParseResourcePathInput ( strURL, pResource, strAbsPath ) && pWebBrowser->IsLocal () )
            {
                pWebBrowser->SetTempURL ( strURL );
                lua_pushboolean ( luaVM,  pWebBrowser->LoadURL ( "mtalocal://" + strURL, false, strPostData, bURLEncoded ) );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsBrowserLoading ( lua_State* luaVM )
{
//  bool isBrowserLoading(browser webBrowser)
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->IsLoading () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserMouseMove ( lua_State* luaVM )
{
//  bool injectBrowserMouseMove(browser webBrowser, int x, int y)
    CClientWebBrowser* pWebBrowser; CVector2D vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadVector2D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseMove ( (int)vecPosition.fX, (int)vecPosition.fY );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserMouseDown ( lua_State* luaVM )
{
//  bool injectBrowserMouseDown ( browser webBrowser, string mouseButton )
    CClientWebBrowser* pWebBrowser; eWebBrowserMouseButton mouseButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadEnumString ( mouseButton );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseDown ( mouseButton );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserMouseUp ( lua_State* luaVM )
{
//  bool injectBrowserMouseUp ( browser webBrowser, string mouseButton )
    CClientWebBrowser* pWebBrowser; eWebBrowserMouseButton mouseButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadEnumString ( mouseButton );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseUp ( mouseButton );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserMouseWheel ( lua_State* luaVM )
{
//  bool injectMouseWheel ( browser webBrowser, int scrollVertical, int scrollHorizontal )
    CClientWebBrowser* pWebBrowser; int iScrollVert; int iScrollHorz;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( iScrollVert );
    argStream.ReadNumber ( iScrollHorz );

    if ( !argStream.HasErrors() )
    {
        pWebBrowser->InjectMouseWheel ( iScrollVert, iScrollHorz );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBrowserTitle ( lua_State* luaVM )
{
//  string getBrowserPageTitle ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        SString strPageTitle;
        pWebBrowser->GetTitle ( strPageTitle );

        lua_pushstring ( luaVM, strPageTitle );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBrowserURL ( lua_State* luaVM )
{
//  string getBrowserURL ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        SString strURL;
        pWebBrowser->GetURL ( strURL );

        lua_pushstring ( luaVM, strURL );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetBrowserRenderingPaused ( lua_State* luaVM )
{
//  bool setBrowserRenderingPaused ( browser webBrowser, bool paused )
    CClientWebBrowser* pWebBrowser; bool bPaused;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadBool ( bPaused );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->SetRenderingPaused ( bPaused );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ExecuteBrowserJavascript ( lua_State* luaVM )
{
//  bool executeBrowserJavascript ( browser webBrowser, string jsCode )
    CClientWebBrowser* pWebBrowser; SString strJavascriptCode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strJavascriptCode );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->ExecuteJavascript ( strJavascriptCode ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetBrowserVolume ( lua_State* luaVM )
{
//  bool setBrowserVolume ( float volume )
//  bool setBrowserVolume ( browser webBrowser, float volume )
    CClientWebBrowser* pWebBrowser; float fVolume;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber ( fVolume );
        lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->SetGlobalAudioVolume ( fVolume ) );
        return 1;
    }
    
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( fVolume );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->SetAudioVolume ( fVolume ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsBrowserDomainBlocked ( lua_State* luaVM )
{
//  bool isBrowserDomainBlocked ( string domain, bool isURL )
    SString strURL; bool bIsURL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strURL );
    argStream.ReadBool ( bIsURL, false );

    if ( !argStream.HasErrors () )
    {
        if ( bIsURL )
            strURL = g_pCore->GetWebCore ()->GetDomainFromURL ( strURL );

        if ( !strURL.empty () )
        {
            lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->GetURLState ( strURL ) != eURLState::WEBPAGE_ALLOWED );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::FocusBrowser ( lua_State* luaVM )
{
//  focusBrowser ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsNil () || argStream.NextIsNone () )
    {
        g_pCore->GetWebCore ()->SetFocusedWebView ( NULL );
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->Focus ();
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsBrowserFocused ( lua_State* luaVM )
{
//  browser isBrowserFocused ()
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        CWebViewInterface* pWebView = g_pCore->GetWebCore ()->GetFocusedWebView ();
        lua_pushboolean ( luaVM, pWebBrowser->GetWebView () == pWebView );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::SetBrowserProperty ( lua_State* luaVM )
{
//  bool setBrowserProperty ( browser webBrowser, string key, string value )
    CClientWebBrowser* pWebBrowser; SString strKey; SString strValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strValue );

    if ( !argStream.HasErrors () )
    {
        if ( pWebBrowser->SetProperty ( strKey, strValue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBrowserProperty ( lua_State* luaVM )
{
//  string getBrowserProperty ( browser webBrowser, string key )
    CClientWebBrowser* pWebBrowser; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        SString strValue;
        if ( pWebBrowser->GetProperty ( strKey, strValue ) )
        {
            lua_pushstring ( luaVM, strValue );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::GUICreateBrowser ( lua_State* luaVM )
{
//  element guiCreateBrowser ( float x, float y, float width, float height, bool isLocal, bool isTransparent, bool relative, [element parent = nil] )
    float x; float y; float width; float height; bool bIsLocal; bool bIsTransparent; bool bIsRelative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( bIsLocal );
    argStream.ReadBool ( bIsTransparent );
    argStream.ReadBool ( bIsRelative );
    argStream.ReadUserData ( parent, nullptr );

    if ( !argStream.HasErrors () )
    {
        if ( !bIsLocal && !g_pCore->GetWebCore()->CanLoadRemotePages () )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateBrowser ( *pLuaMain, x, y, width, height, bIsLocal, bIsTransparent, bIsRelative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIGetBrowser ( lua_State* luaVM ) // Or rather guiGetBrowserBrowser?
{
//  webbrowser guiGetBrowser ( gui-webbrowser browser )
    CClientGUIElement* pGUIElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIWebBrowser > ( pGUIElement );

    if ( !argStream.HasErrors () )
    {
        if ( IS_GUI ( pGUIElement ) && pGUIElement->GetCGUIType () == CGUI_WEBBROWSER )
        {
            CClientGUIWebBrowser* pGUIBrowser = static_cast < CClientGUIWebBrowser* > ( pGUIElement );
            lua_pushelement ( luaVM, pGUIBrowser->GetBrowser () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
